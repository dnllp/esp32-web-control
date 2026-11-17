// Valores por defecto de la ROmBA.
//
// Se usan la primera vez que arranca la ESP32 (o tras "Restaurar valores de
// fábrica"). Después, todo se cambia desde la página web y se guarda en NVS,
// sin recompilar.
#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------- Pines
#if CONFIG_IDF_TARGET_ESP32S3
  #define BOARD_NAME "ESP32-S3"
  // Evitados en el S3: 0, 3, 45, 46 (arranque), 19-20 (USB),
  // 26-32 (flash) y 33-37 (PSRAM en módulos N8R8 / N16R8).
  #define DEF_PIN_ENA    4
  #define DEF_PIN_IN1    5
  #define DEF_PIN_IN2    6
  #define DEF_PIN_ENB    7
  #define DEF_PIN_IN3   15
  #define DEF_PIN_IN4   16
  #define DEF_PIN_ENC_LA 17
  #define DEF_PIN_ENC_LB 18
  #define DEF_PIN_ENC_RA 12
  #define DEF_PIN_ENC_RB 13
  #define DEF_PIN_SDA    8
  #define DEF_PIN_SCL    9
#else
  #define BOARD_NAME "ESP32 DevKit V1"
  // GPIO 34-39 solo son entradas y NO tienen pull-up interno: los encoders
  // necesitan pull-up externo (casi todos los módulos ya lo traen).
  // GPIO 12 es pin de arranque: si el puente H lo mantiene en ALTO al
  // encender, la ESP32 no arranca. Con L298N normalmente no pasa.
  #define DEF_PIN_ENA   25
  #define DEF_PIN_IN1   26
  #define DEF_PIN_IN2   27
  #define DEF_PIN_ENB   14
  #define DEF_PIN_IN3   12
  #define DEF_PIN_IN4   13
  #define DEF_PIN_ENC_LA 34
  #define DEF_PIN_ENC_LB 35
  #define DEF_PIN_ENC_RA 32
  #define DEF_PIN_ENC_RB 33
  #define DEF_PIN_SDA   21
  #define DEF_PIN_SCL   22
#endif

// ---------------------------------------------------------------- WiFi (AP)
#define DEF_AP_SSID     "ROmBA"
#define DEF_AP_PASS     "romba1234"   // vacío = red abierta; si no, mínimo 8 caracteres
#define DEF_AP_CHANNEL  6

// ---------------------------------------------------------------- Robot
// Robot de ~30 cm de diámetro con ruedas de 130 mm.
#define DEF_ROBOT_DIAM_MM   300.0f
#define DEF_WHEEL_DIAM_MM   130.0f
#define DEF_WHEEL_BASE_MM   230.0f   // distancia entre los centros de las ruedas
#define DEF_MAX_SPEED       0.40f    // m/s con el joystick al 100 %
#define DEF_MAX_ANGULAR     2.00f    // rad/s con el joystick al 100 %
#define DEF_ACCEL_MAX       0.80f    // m/s^2 por rueda (0 = sin rampa)

// ---------------------------------------------------------------- Encoders
#define ENC_SIMPLE      0
#define ENC_QUADRATURE  1
#define DEF_ENC_TYPE    ENC_QUADRATURE
#define DEF_PPR_SIMPLE  20      // pulsos por vuelta de RUEDA (disco ranurado típico)
#define DEF_PPR_QUAD    360     // pulsos por vuelta de RUEDA del canal A (cuenta x1)
#define DEF_GEAR_RATIO  1.0f    // vueltas del eje del encoder por vuelta de rueda
#define DEF_ENC_FILTER_US 100   // ignora pulsos más cercanos que esto (rebotes)

// ---------------------------------------------------------------- IMU
#define IMU_NONE     0
#define IMU_BNO055   1
#define IMU_MPU6050  2
#define DEF_IMU_TYPE     IMU_MPU6050 
#define DEF_IMU_ADDR_BNO 0x28
#define DEF_IMU_ADDR_MPU 0x68
#define DEF_IMU_FUSION   true
#define DEF_IMU_ALPHA    0.80f   // peso de la IMU en el ángulo (0 = solo encoders, 1 = solo IMU)

// ---------------------------------------------------------------- Motores / PID
#define DEF_PWM_FREQ     1000
#define DEF_PWM_BITS     8
#define DEF_PWM_MIN_PCT  25.0f   // PWM con el que la rueda empieza a moverse (zona muerta)
#define DEF_PID_ENABLED  true
#define DEF_KP           60.0f   // % de PWM por cada m/s de error
#define DEF_KI           250.0f
#define DEF_KD           0.0f

// ---------------------------------------------------------------- Control
#define DEF_WS_TIMEOUT_MS 500    // sin comandos durante este tiempo -> paro
#define DEF_TELEMETRY_HZ  10
#define DEF_CONTROL_HZ    50
