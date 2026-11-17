// IMU seleccionable desde la configuración: BNO055 o MPU6050 (por I2C).
//
// Convención (la de ROS): yaw positivo = giro antihorario visto desde arriba.
// La IMU debe ir con el eje Z hacia arriba; si va al revés, activa
// "Invertir yaw" en la configuración.
#pragma once
#include <Wire.h>
#include "Config.h"

class Imu {
 public:
  bool begin(const Config& cfg, TwoWire& wire);
  void update();       // llamar a ~100 Hz
  bool ok() const { return ok_; }
  bool present() const { return present_; }
  const char* name() const;

  // Giro acumulado (rad) desde la última llamada. Lo usa la odometría.
  float takeYawDelta();
  // Calibra el offset del giroscopio (MPU6050). El robot debe estar quieto.
  void calibrateGyro();

  float yawDeg = 0, rollDeg = 0, pitchDeg = 0;
  float gyroZ = 0;              // rad/s
  float ax = 0, ay = 0, az = 0; // m/s^2
  uint8_t calib = 0;            // BNO055: byte CALIB_STAT (sys|gyr|acc|mag, 2 bits c/u)

 private:
  bool write8(uint8_t reg, uint8_t val);
  bool read(uint8_t reg, uint8_t* buf, size_t n);
  bool beginBno();
  bool beginMpu();
  bool updateBno();
  bool updateMpu(float dt);
  void addYaw(float newYawDeg);

  TwoWire* wire_ = nullptr;
  uint8_t type_ = IMU_NONE, addr_ = 0;
  bool ok_ = false, present_ = false, invert_ = false, first_ = true;
  uint8_t errors_ = 0;
  float prevYawDeg_ = 0, yawAccumRad_ = 0;
  float gyroBias_[3] = {0, 0, 0};
  uint32_t lastUs_ = 0;
};
