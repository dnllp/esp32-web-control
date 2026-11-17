#include "Imu.h"

// ---- Registros BNO055
static const uint8_t BNO_CHIP_ID = 0x00, BNO_PAGE_ID = 0x07, BNO_ACC_DATA = 0x08,
                     BNO_GYR_DATA = 0x14, BNO_EUL_DATA = 0x1A, BNO_CALIB_STAT = 0x35,
                     BNO_UNIT_SEL = 0x3B, BNO_OPR_MODE = 0x3D, BNO_PWR_MODE = 0x3E,
                     BNO_SYS_TRIGGER = 0x3F;
static const uint8_t BNO_MODE_CONFIG = 0x00;
// IMUPLUS: fusión acelerómetro + giroscopio, sin magnetómetro. Los motores
// distorsionan mucho el campo magnético, así que NDOF daría saltos en el ángulo.
static const uint8_t BNO_MODE_IMUPLUS = 0x08;

// ---- Registros MPU6050
static const uint8_t MPU_SMPLRT_DIV = 0x19, MPU_CONFIG = 0x1A, MPU_GYRO_CONFIG = 0x1B,
                     MPU_ACCEL_CONFIG = 0x1C, MPU_ACCEL_OUT = 0x3B, MPU_PWR_MGMT_1 = 0x6B,
                     MPU_WHO_AM_I = 0x75;
static const float MPU_GYRO_LSB = 65.5f;     // ±500 °/s
static const float MPU_ACC_LSB = 16384.0f;   // ±2 g
static const float G = 9.80665f;

static float wrap180(float d) {
  while (d > 180) d -= 360;
  while (d < -180) d += 360;
  return d;
}

bool Imu::write8(uint8_t reg, uint8_t val) {
  wire_->beginTransmission(addr_);
  wire_->write(reg);
  wire_->write(val);
  return wire_->endTransmission() == 0;
}

bool Imu::read(uint8_t reg, uint8_t* buf, size_t n) {
  wire_->beginTransmission(addr_);
  wire_->write(reg);
  if (wire_->endTransmission(false) != 0) return false;
  if (wire_->requestFrom(addr_, (uint8_t)n) != n) return false;
  for (size_t i = 0; i < n; i++) buf[i] = wire_->read();
  return true;
}

const char* Imu::name() const {
  switch (type_) {
    case IMU_BNO055: return "BNO055";
    case IMU_MPU6050: return "MPU6050";
    default: return "ninguna";
  }
}

bool Imu::begin(const Config& cfg, TwoWire& wire) {
  wire_ = &wire;
  type_ = cfg.imuType;
  invert_ = cfg.imuInvertYaw;
  ok_ = false;
  if (type_ == IMU_BNO055) {
    addr_ = cfg.imuAddrBno;
    ok_ = beginBno();
  } else if (type_ == IMU_MPU6050) {
    addr_ = cfg.imuAddrMpu;
    ok_ = beginMpu();
  }
  present_ = ok_;
  first_ = true;
  lastUs_ = micros();
  return ok_;
}

// ------------------------------------------------------------------ BNO055
bool Imu::beginBno() {
  uint8_t id = 0;
  // Tras encender, el BNO055 tarda hasta ~850 ms en responder.
  for (int i = 0; i < 20 && !(read(BNO_CHIP_ID, &id, 1) && id == 0xA0); i++) delay(50);
  if (id != 0xA0) return false;
  write8(BNO_OPR_MODE, BNO_MODE_CONFIG);
  delay(25);
  write8(BNO_SYS_TRIGGER, 0x20);  // reset
  delay(50);
  for (int i = 0; i < 20 && !(read(BNO_CHIP_ID, &id, 1) && id == 0xA0); i++) delay(50);
  write8(BNO_PWR_MODE, 0x00);  // normal
  write8(BNO_PAGE_ID, 0x00);
  write8(BNO_UNIT_SEL, 0x00);  // m/s^2, °/s, grados
  write8(BNO_SYS_TRIGGER, 0x00);
  delay(10);
  bool ok = write8(BNO_OPR_MODE, BNO_MODE_IMUPLUS);
  delay(20);
  return ok;
}

bool Imu::updateBno() {
  uint8_t b[6];
  if (!read(BNO_EUL_DATA, b, 6)) return false;
  float heading = (int16_t)(b[0] | b[1] << 8) / 16.0f;  // 0..360, sentido horario
  rollDeg = (int16_t)(b[2] | b[3] << 8) / 16.0f;
  pitchDeg = (int16_t)(b[4] | b[5] << 8) / 16.0f;
  addYaw(-heading);
  if (!read(BNO_GYR_DATA, b, 6)) return false;
  gyroZ = (int16_t)(b[4] | b[5] << 8) / 16.0f * DEG_TO_RAD;
  if (!read(BNO_ACC_DATA, b, 6)) return false;
  ax = (int16_t)(b[0] | b[1] << 8) / 100.0f;
  ay = (int16_t)(b[2] | b[3] << 8) / 100.0f;
  az = (int16_t)(b[4] | b[5] << 8) / 100.0f;
  read(BNO_CALIB_STAT, &calib, 1);
  return true;
}

// ------------------------------------------------------------------ MPU6050
bool Imu::beginMpu() {
  uint8_t id = 0;
  if (!read(MPU_WHO_AM_I, &id, 1)) return false;
  // 0x68 = MPU6050 original. Los clones devuelven 0x70, 0x72, 0x98...: se aceptan.
  if (id == 0x00 || id == 0xFF) return false;
  write8(MPU_PWR_MGMT_1, 0x80);  // reset
  delay(100);
  write8(MPU_PWR_MGMT_1, 0x01);  // reloj = PLL del giro X
  write8(MPU_CONFIG, 0x03);      // filtro pasa bajas ~44 Hz
  write8(MPU_SMPLRT_DIV, 4);     // 200 Hz
  write8(MPU_GYRO_CONFIG, 0x08); // ±500 °/s
  bool ok = write8(MPU_ACCEL_CONFIG, 0x00);  // ±2 g
  delay(50);
  if (ok) {
    ok_ = true;
    calibrateGyro();
  }
  return ok;
}

void Imu::calibrateGyro() {
  if (type_ != IMU_MPU6050 || !ok_) return;
  float sum[3] = {0, 0, 0};
  int n = 0;
  uint8_t b[6];
  for (int i = 0; i < 400; i++) {
    if (read(0x43, b, 6)) {
      for (int k = 0; k < 3; k++) sum[k] += (int16_t)(b[2 * k] << 8 | b[2 * k + 1]);
      n++;
    }
    delay(3);
  }
  if (n > 0)
    for (int k = 0; k < 3; k++) gyroBias_[k] = sum[k] / n;
  lastUs_ = micros();
}

bool Imu::updateMpu(float dt) {
  uint8_t b[14];
  if (!read(MPU_ACCEL_OUT, b, 14)) return false;
  auto s16 = [&](int i) { return (int16_t)(b[i] << 8 | b[i + 1]); };
  ax = s16(0) / MPU_ACC_LSB * G;
  ay = s16(2) / MPU_ACC_LSB * G;
  az = s16(4) / MPU_ACC_LSB * G;
  float gx = (s16(8) - gyroBias_[0]) / MPU_GYRO_LSB;   // °/s
  float gy = (s16(10) - gyroBias_[1]) / MPU_GYRO_LSB;
  float gz = (s16(12) - gyroBias_[2]) / MPU_GYRO_LSB;
  if (fabsf(gz) < 0.15f) gz = 0;  // banda muerta: evita que el ángulo derive en reposo
  gyroZ = gz * DEG_TO_RAD;
  // Roll / pitch: filtro complementario giro + acelerómetro.
  float accRoll = atan2f(ay, az) * RAD_TO_DEG;
  float accPitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * RAD_TO_DEG;
  rollDeg = 0.98f * (rollDeg + gx * dt) + 0.02f * accRoll;
  pitchDeg = 0.98f * (pitchDeg + gy * dt) + 0.02f * accPitch;
  // Yaw: solo se puede integrar el giroscopio (deriva lentamente).
  addYaw(yawDeg * (invert_ ? -1 : 1) + gz * dt);
  return true;
}

// ------------------------------------------------------------------ común
void Imu::addYaw(float newYawDeg) {
  if (invert_) newYawDeg = -newYawDeg;
  if (first_) {
    prevYawDeg_ = newYawDeg;
    first_ = false;
  }
  yawAccumRad_ += wrap180(newYawDeg - prevYawDeg_) * DEG_TO_RAD;
  prevYawDeg_ = newYawDeg;
  yawDeg = wrap180(newYawDeg);
}

void Imu::update() {
  if (type_ == IMU_NONE || !present_) return;
  uint32_t now = micros();
  float dt = (now - lastUs_) * 1e-6f;
  lastUs_ = now;
  bool good = type_ == IMU_BNO055 ? updateBno() : updateMpu(dt);
  if (good) {
    if (invert_) gyroZ = -gyroZ;
    errors_ = 0;
    ok_ = true;
  } else if (++errors_ > 10) {
    ok_ = false;  // la odometría deja de usarla hasta que vuelva a responder
  }
}

float Imu::takeYawDelta() {
  float d = yawAccumRad_;
  yawAccumRad_ = 0;
  return d;
}
