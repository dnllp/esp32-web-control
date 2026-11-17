// Dos motores DC con puente H doble (L298N, L9110, TB6612...).
//   Izquierdo: ENA (PWM), IN1, IN2     Derecho: ENB (PWM), IN3, IN4
#pragma once
#include "Config.h"

class Motors {
 public:
  bool begin(const Config& cfg);
  // pctL / pctR en -100..100. 0 = rueda libre (IN1=IN2=LOW).
  void set(float pctL, float pctR);
  void stop() { set(0, 0); }
  float pctL() const { return pctL_; }
  float pctR() const { return pctR_; }

 private:
  void apply(int side, float pct, bool invert, uint8_t pinEn, uint8_t inA, uint8_t inB);
  void writeDuty(int side, uint8_t pin, uint32_t duty);
  const Config* cfg_ = nullptr;
  uint32_t maxDuty_ = 255;
  float pctL_ = 0, pctR_ = 0;
  bool ready_ = false;
};
