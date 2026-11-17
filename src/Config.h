// Configuración completa del robot. Se guarda en NVS (memoria no volátil)
// y se edita desde la pestaña "Configuración" de la página web.
//
// Para añadir un parámetro basta con agregar una línea a CONFIG_FIELDS:
// el struct, los valores por defecto y el JSON se generan solos. La página web
// también debe conocerlo (arreglo SCHEMA en web/index.html).
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "defaults.h"

#define CONFIG_FIELDS(X)                                   \
  /* WiFi */                                               \
  X(uint8_t,  apChannel,     DEF_AP_CHANNEL)               \
  /* Pines */                                              \
  X(uint8_t,  pinEna,        DEF_PIN_ENA)                  \
  X(uint8_t,  pinIn1,        DEF_PIN_IN1)                  \
  X(uint8_t,  pinIn2,        DEF_PIN_IN2)                  \
  X(uint8_t,  pinEnb,        DEF_PIN_ENB)                  \
  X(uint8_t,  pinIn3,        DEF_PIN_IN3)                  \
  X(uint8_t,  pinIn4,        DEF_PIN_IN4)                  \
  X(uint8_t,  pinEncLA,      DEF_PIN_ENC_LA)               \
  X(uint8_t,  pinEncLB,      DEF_PIN_ENC_LB)               \
  X(uint8_t,  pinEncRA,      DEF_PIN_ENC_RA)               \
  X(uint8_t,  pinEncRB,      DEF_PIN_ENC_RB)               \
  X(uint8_t,  pinSda,        DEF_PIN_SDA)                  \
  X(uint8_t,  pinScl,        DEF_PIN_SCL)                  \
  /* Motores */                                            \
  X(uint16_t, pwmFreq,       DEF_PWM_FREQ)                 \
  X(uint8_t,  pwmBits,       DEF_PWM_BITS)                 \
  X(float,    pwmMinPct,     DEF_PWM_MIN_PCT)              \
  X(bool,     invertMotorL,  false)                        \
  X(bool,     invertMotorR,  false)                        \
  /* Encoders */                                           \
  X(uint8_t,  encType,       DEF_ENC_TYPE)                 \
  X(uint16_t, pprSimple,     DEF_PPR_SIMPLE)               \
  X(uint16_t, pprQuad,       DEF_PPR_QUAD)                 \
  X(float,    gearRatio,     DEF_GEAR_RATIO)               \
  X(uint16_t, encFilterUs,   DEF_ENC_FILTER_US)            \
  X(bool,     invertEncL,    false)                        \
  X(bool,     invertEncR,    false)                        \
  /* IMU */                                                \
  X(uint8_t,  imuType,       DEF_IMU_TYPE)                 \
  X(uint8_t,  imuAddrBno,    DEF_IMU_ADDR_BNO)             \
  X(uint8_t,  imuAddrMpu,    DEF_IMU_ADDR_MPU)             \
  X(bool,     imuFusion,     DEF_IMU_FUSION)               \
  X(float,    imuAlpha,      DEF_IMU_ALPHA)                \
  X(bool,     imuInvertYaw,  false)                        \
  /* Robot (odometría) */                                  \
  X(float,    robotDiamMm,   DEF_ROBOT_DIAM_MM)            \
  X(float,    wheelDiamMm,   DEF_WHEEL_DIAM_MM)            \
  X(float,    wheelBaseMm,   DEF_WHEEL_BASE_MM)            \
  X(float,    maxSpeed,      DEF_MAX_SPEED)                \
  X(float,    maxAngular,    DEF_MAX_ANGULAR)              \
  X(float,    accelMax,      DEF_ACCEL_MAX)                \
  /* PID */                                                \
  X(bool,     pidEnabled,    DEF_PID_ENABLED)              \
  X(float,    kp,            DEF_KP)                       \
  X(float,    ki,            DEF_KI)                       \
  X(float,    kd,            DEF_KD)                       \
  /* Control */                                            \
  X(uint16_t, wsTimeoutMs,   DEF_WS_TIMEOUT_MS)            \
  X(uint8_t,  telemetryHz,   DEF_TELEMETRY_HZ)             \
  X(uint8_t,  controlHz,     DEF_CONTROL_HZ)

struct Config {
  char apSsid[33];
  char apPass[65];
#define X_DECL(type, name, def) type name;
  CONFIG_FIELDS(X_DECL)
#undef X_DECL

  void setDefaults();
  void toJson(JsonObject obj) const;
  // Copia los campos presentes en obj; devuelve texto de error o "" si es válido.
  String fromJson(JsonObjectConst obj);
  String validate() const;

  // Valores derivados para la odometría
  uint16_t ppr() const { return encType == ENC_QUADRATURE ? pprQuad : pprSimple; }
  float ticksPerRev() const { return ppr() * gearRatio; }
  float wheelCircM() const { return PI * wheelDiamMm / 1000.0f; }
  float metersPerTick() const { return wheelCircM() / ticksPerRev(); }
  float wheelBaseM() const { return wheelBaseMm / 1000.0f; }
};

namespace ConfigStore {
// Devuelve true si había una configuración guardada (el robot ya fue configurado).
bool load(Config& cfg);
bool save(const Config& cfg);
void erase();
}
