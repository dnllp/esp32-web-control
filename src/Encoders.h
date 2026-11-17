// Encoders de rueda, por interrupción en el flanco de subida del canal A (x1).
//
//  - Cuadratura: el sentido se lee del canal B en el momento del flanco.
//  - Simple (un solo canal): no puede saber el sentido, así que lo toma del
//    sentido que se le está mandando al motor (setCommandSign).
//
// La velocidad se calcula con el tiempo entre pulsos, que da buena
// resolución aunque haya pocos pulsos por vuelta (p. ej. un disco de 20 ranuras).
#pragma once
#include "Config.h"

class Encoders {
 public:
  enum Side { LEFT = 0, RIGHT = 1 };

  void begin(const Config& cfg);
  int32_t count(Side s) const;
  // Velocidad de la rueda en vueltas por segundo (con signo).
  float revPerSec(Side s) const;
  // Solo para encoder simple: sentido que se está mandando al motor (-1, 0, 1).
  void setCommandSign(Side s, int8_t sign);
  void reset();

  struct State {
    volatile int32_t count = 0;
    volatile uint32_t lastUs = 0;
    volatile uint32_t periodUs = 0;
    volatile int8_t lastDir = 1;
    volatile int8_t cmdSign = 1;  // último sentido distinto de 0 que se mandó
    uint8_t pinB = 0;
    bool quad = false;
    int8_t invert = 1;
    uint16_t filterUs = 0;
  };

 private:
  State st_[2];
  float ticksPerRev_ = 1;
};
