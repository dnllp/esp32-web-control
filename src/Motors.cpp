#include "Motors.h"
#if __has_include(<esp_arduino_version.h>)
#include <esp_arduino_version.h>
#endif
#ifndef ESP_ARDUINO_VERSION_MAJOR
#define ESP_ARDUINO_VERSION_MAJOR 2
#endif

bool Motors::begin(const Config& cfg) {
  cfg_ = &cfg;
  maxDuty_ = (1u << cfg.pwmBits) - 1;
  for (uint8_t p : {cfg.pinIn1, cfg.pinIn2, cfg.pinIn3, cfg.pinIn4}) {
    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
  }
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ready_ = ledcAttach(cfg.pinEna, cfg.pwmFreq, cfg.pwmBits) &&
           ledcAttach(cfg.pinEnb, cfg.pwmFreq, cfg.pwmBits);
#else
  ready_ = ledcSetup(0, cfg.pwmFreq, cfg.pwmBits) > 0 && ledcSetup(1, cfg.pwmFreq, cfg.pwmBits) > 0;
  ledcAttachPin(cfg.pinEna, 0);
  ledcAttachPin(cfg.pinEnb, 1);
#endif
  stop();
  return ready_;
}

void Motors::writeDuty(int side, uint8_t pin, uint32_t duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  (void)side;
  ledcWrite(pin, duty);
#else
  (void)pin;
  ledcWrite(side, duty);
#endif
}

void Motors::apply(int side, float pct, bool invert, uint8_t pinEn, uint8_t inA, uint8_t inB) {
  if (invert) pct = -pct;
  float mag = fabsf(pct);
  if (mag < 0.5f) {  // rueda libre
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
    writeDuty(side, pinEn, 0);
    return;
  }
  if (mag > 100) mag = 100;
  // Compensa la zona muerta: 1 % ya equivale al PWM mínimo que mueve la rueda.
  float out = cfg_->pwmMinPct + mag * (100.0f - cfg_->pwmMinPct) / 100.0f;
  digitalWrite(inA, pct > 0 ? HIGH : LOW);
  digitalWrite(inB, pct > 0 ? LOW : HIGH);
  writeDuty(side, pinEn, (uint32_t)(out / 100.0f * maxDuty_ + 0.5f));
}

void Motors::set(float pctL, float pctR) {
  pctL_ = constrain(pctL, -100.0f, 100.0f);
  pctR_ = constrain(pctR, -100.0f, 100.0f);
  if (!ready_) return;
  apply(0, pctL_, cfg_->invertMotorL, cfg_->pinEna, cfg_->pinIn1, cfg_->pinIn2);
  apply(1, pctR_, cfg_->invertMotorR, cfg_->pinEnb, cfg_->pinIn3, cfg_->pinIn4);
}
