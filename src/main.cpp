// ROmBA - control web con ESP32
//
// La ESP32 crea su propia red WiFi (modo AP), sirve una página web y recibe
// los comandos de movimiento por WebSocket. Calcula la odometría con los
// encoders (y opcionalmente la IMU) y la manda a la página.
//
// Toda la configuración (pines, tipo de encoder, tipo de IMU, medidas del
// robot, PID...) se guarda en NVS y se cambia desde la propia página.
#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <Wire.h>

#include "Config.h"
#include "Encoders.h"
#include "Imu.h"
#include "Motors.h"
#include "Odometry.h"
#include "Pid.h"
#include "WebUi.h"

static Config cfg;
static bool configured = false;
static Motors motors;
static Encoders encoders;
static Imu imu;
static Odometry odom;
static Pid pidL, pidR;
static WebUi web;
static RemoteState remote;

// Estado del lazo de control
static float targetL = 0, targetR = 0;  // m/s por rueda (después de la rampa)
static float measL = 0, measR = 0;      // m/s medidos
static int32_t prevTicksL = 0, prevTicksR = 0;
static bool timedOut = true;
static uint32_t lastControlUs = 0, lastImuMs = 0, lastTelMs = 0;

static void applyPidGains() {
  for (Pid* p : {&pidL, &pidR}) {
    p->kp = cfg.kp;
    p->ki = cfg.ki;
    p->kd = cfg.kd;
    p->reset();
  }
}

static float rampTo(float current, float target, float maxStep) {
  if (maxStep <= 0) return target;
  if (target > current + maxStep) return current + maxStep;
  if (target < current - maxStep) return current - maxStep;
  return target;
}

static void startWifi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(cfg.apSsid, strlen(cfg.apPass) ? cfg.apPass : nullptr, cfg.apChannel);
  WiFi.setSleep(false);  // menos latencia en el WebSocket
  if (MDNS.begin("romba")) MDNS.addService("http", "tcp", 80);
  Serial.printf("[wifi] red \"%s\"  ->  http://%s  (o http://romba.local)\n", cfg.apSsid,
                WiFi.softAPIP().toString().c_str());
}

static void controlStep(float dt) {
  // ---- 1. Comando de la página (con paro si se pierde la conexión)
  if (remote.stopNow) {
    remote.stopNow = false;
    remote.hasCmd = false;
    targetL = targetR = 0;
  }
  bool fresh = remote.hasCmd && (millis() - remote.lastCmdMs) < cfg.wsTimeoutMs;
  if (!fresh && !timedOut) Serial.println("[ctl] sin comandos: paro");
  timedOut = !fresh;
  float v = fresh ? remote.v * cfg.maxSpeed : 0;
  float w = fresh ? remote.w * cfg.maxAngular : 0;

  // Cinemática inversa del robot diferencial
  float half = cfg.wheelBaseM() / 2;
  float maxStep = cfg.accelMax * dt;
  if (timedOut) maxStep *= 3;  // al perder conexión frena más rápido
  targetL = rampTo(targetL, v - w * half, maxStep);
  targetR = rampTo(targetR, v + w * half, maxStep);

  // ---- 2. Medición de encoders
  float circ = cfg.wheelCircM();
  measL = 0.6f * measL + 0.4f * encoders.revPerSec(Encoders::LEFT) * circ;
  measR = 0.6f * measR + 0.4f * encoders.revPerSec(Encoders::RIGHT) * circ;

  // ---- 3. Salida a motores: prealimentación + PID
  float maxWheel = cfg.maxSpeed + cfg.maxAngular * half;
  float outL = 0, outR = 0;
  bool stopped = fabsf(targetL) < 0.005f && fabsf(targetR) < 0.005f;
  if (stopped) {
    pidL.reset();
    pidR.reset();
  } else {
    float ffL = targetL / maxWheel * 100, ffR = targetR / maxWheel * 100;
    if (cfg.pidEnabled) {
      outL = pidL.update(targetL, measL, ffL, dt);
      outR = pidR.update(targetR, measR, ffR, dt);
    } else {
      outL = ffL;
      outR = ffR;
    }
  }
  motors.set(outL, outR);
  // El encoder simple necesita saber hacia dónde se manda cada rueda
  encoders.setCommandSign(Encoders::LEFT, outL > 0 ? 1 : (outL < 0 ? -1 : 0));
  encoders.setCommandSign(Encoders::RIGHT, outR > 0 ? 1 : (outR < 0 ? -1 : 0));

  // ---- 4. Odometría
  int32_t tL = encoders.count(Encoders::LEFT), tR = encoders.count(Encoders::RIGHT);
  float mpt = cfg.metersPerTick();
  float dL = (tL - prevTicksL) * mpt, dR = (tR - prevTicksR) * mpt;
  prevTicksL = tL;
  prevTicksR = tR;
  float imuD = imu.takeYawDelta();
  bool useImu = cfg.imuFusion && imu.ok();
  odom.update(dL, dR, cfg.wheelBaseM(), dt, useImu, imuD, cfg.imuAlpha);
}

static void sendTelemetry() {
  JsonDocument d;
  d["t"] = "tel";
  d["ms"] = millis();
  d["x"] = odom.x;
  d["y"] = odom.y;
  d["th"] = odom.theta;
  d["v"] = odom.v;
  d["w"] = odom.w;
  d["dist"] = odom.dist;
  JsonObject wl = d["wl"].to<JsonObject>();
  wl["tgt"] = targetL;
  wl["meas"] = measL;
  wl["pwm"] = motors.pctL();
  wl["ticks"] = encoders.count(Encoders::LEFT);
  JsonObject wr = d["wr"].to<JsonObject>();
  wr["tgt"] = targetR;
  wr["meas"] = measR;
  wr["pwm"] = motors.pctR();
  wr["ticks"] = encoders.count(Encoders::RIGHT);
  JsonObject im = d["imu"].to<JsonObject>();
  im["type"] = imu.name();
  im["ok"] = imu.ok();
  im["fusion"] = cfg.imuFusion && imu.ok();
  im["yaw"] = imu.yawDeg;
  im["roll"] = imu.rollDeg;
  im["pitch"] = imu.pitchDeg;
  im["gz"] = imu.gyroZ;
  im["ax"] = imu.ax;
  im["ay"] = imu.ay;
  im["az"] = imu.az;
  im["cal"] = imu.calib;
  d["pid"] = cfg.pidEnabled;
  d["timeout"] = timedOut;
  d["heap"] = ESP.getFreeHeap();
  d["sta"] = WiFi.softAPgetStationNum();
  String s;
  serializeJson(d, s);
  web.broadcast(s);
}

static void handleRequests() {
  if (remote.resetOdom) {
    remote.resetOdom = false;
    odom.reset();
    imu.takeYawDelta();
    web.message("ok", "Odometría reiniciada");
  }
  if (remote.imuCal) {
    remote.imuCal = false;
    motors.stop();
    if (cfg.imuType == IMU_MPU6050) {
      web.message("warn", "Calibrando giroscopio, no muevas el robot...");
      imu.calibrateGyro();
      web.message("ok", "Giroscopio calibrado");
    } else {
      web.message("warn", "El BNO055 se calibra solo: gíralo despacio y déjalo quieto unos segundos");
    }
  }
  if (remote.pendingPid) {
    remote.pendingPid = false;
    cfg.pidEnabled = remote.pidEn;
    cfg.kp = remote.kp;
    cfg.ki = remote.ki;
    cfg.kd = remote.kd;
    applyPidGains();
    if (remote.pidSave) {
      ConfigStore::save(cfg);
      web.message("ok", "PID guardado en memoria");
    }
  }
  if (remote.pendingCfg) {
    motors.stop();
    bool ok = ConfigStore::save(remote.newCfg);
    web.message(ok ? "ok" : "err", ok ? "Configuración guardada. Reiniciando..." : "No se pudo guardar en NVS");
    if (ok) {
      delay(500);
      ESP.restart();
    }
    remote.pendingCfg = false;
  }
  if (remote.factoryReset) {
    motors.stop();
    ConfigStore::erase();
    web.message("ok", "Valores de fábrica restaurados. Reiniciando...");
    delay(500);
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.printf("\n=== ROmBA control web (%s) ===\n", BOARD_NAME);

  configured = ConfigStore::load(cfg);
  Serial.println(configured ? "[cfg] configuración cargada de NVS"
                            : "[cfg] sin configuración guardada: valores por defecto");

  if (!motors.begin(cfg)) Serial.println("[mot] error al configurar el PWM");
  encoders.begin(cfg);
  Serial.printf("[enc] %s, %u PPR x %.2f = %.1f ticks/vuelta, %.2f mm/tick\n",
                cfg.encType == ENC_QUADRATURE ? "cuadratura" : "simple", cfg.ppr(), cfg.gearRatio,
                cfg.ticksPerRev(), cfg.metersPerTick() * 1000);

  if (cfg.imuType != IMU_NONE) {
    Wire.begin(cfg.pinSda, cfg.pinScl, 100000);
    Wire.setTimeOut(20);
    bool ok = imu.begin(cfg, Wire);
    Serial.printf("[imu] %s: %s\n", imu.name(), ok ? "OK" : "NO RESPONDE");
  }

  applyPidGains();
  startWifi();
  web.begin(cfg, configured, remote);
  lastControlUs = micros();
}

void loop() {
  web.loop();
  handleRequests();

  uint32_t nowMs = millis();
  if (nowMs - lastImuMs >= 10) {  // IMU a 100 Hz
    lastImuMs = nowMs;
    imu.update();
  }

  uint32_t nowUs = micros();
  uint32_t periodUs = 1000000UL / cfg.controlHz;
  if (nowUs - lastControlUs >= periodUs) {
    float dt = (nowUs - lastControlUs) * 1e-6f;
    lastControlUs = nowUs;
    controlStep(dt);
  }

  if (nowMs - lastTelMs >= 1000UL / cfg.telemetryHz) {
    lastTelMs = nowMs;
    if (web.hasClients()) sendTelemetry();
  }
  delay(1);
}
