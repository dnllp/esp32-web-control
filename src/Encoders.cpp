#include "Encoders.h"
#include <soc/gpio_struct.h>

static portMUX_TYPE encMux = portMUX_INITIALIZER_UNLOCKED;

// Lectura directa del registro: digitalRead() no está en IRAM.
static inline int IRAM_ATTR fastRead(uint8_t pin) {
  if (pin < 32) return (GPIO.in >> pin) & 1;
  return (GPIO.in1.data >> (pin - 32)) & 1;
}

static void IRAM_ATTR onPulse(void* arg) {
  Encoders::State* s = static_cast<Encoders::State*>(arg);
  uint32_t now = micros();
  uint32_t dt = now - s->lastUs;
  if (dt < s->filterUs) return;  // rebote / ruido
  int8_t dir = s->quad ? (fastRead(s->pinB) ? -1 : 1) : s->cmdSign;
  dir *= s->invert;
  portENTER_CRITICAL_ISR(&encMux);
  // Si cambió el sentido, el periodo anterior no sirve para la velocidad.
  s->periodUs = (dir == s->lastDir) ? dt : 0;
  s->lastUs = now;
  s->lastDir = dir;
  s->count += dir;
  portEXIT_CRITICAL_ISR(&encMux);
}

void Encoders::begin(const Config& cfg) {
  ticksPerRev_ = cfg.ticksPerRev();
  const uint8_t pinsA[2] = {cfg.pinEncLA, cfg.pinEncRA};
  const uint8_t pinsB[2] = {cfg.pinEncLB, cfg.pinEncRB};
  const bool inv[2] = {cfg.invertEncL, cfg.invertEncR};
  for (int i = 0; i < 2; i++) {
    State& s = st_[i];
    s.quad = cfg.encType == ENC_QUADRATURE;
    s.pinB = pinsB[i];
    // Con encoder simple el sentido viene del comando del motor: no se invierte.
    s.invert = (s.quad && inv[i]) ? -1 : 1;
    s.filterUs = cfg.encFilterUs;
    s.lastUs = micros();
    pinMode(pinsA[i], INPUT_PULLUP);  // GPIO 34-39 ignoran el pull-up
    if (s.quad) pinMode(pinsB[i], INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(pinsA[i]), onPulse, &s, RISING);
  }
}

int32_t Encoders::count(Side side) const {
  portENTER_CRITICAL(&encMux);
  int32_t c = st_[side].count;
  portEXIT_CRITICAL(&encMux);
  return c;
}

float Encoders::revPerSec(Side side) const {
  const State& s = st_[side];
  portENTER_CRITICAL(&encMux);
  uint32_t last = s.lastUs, period = s.periodUs;
  int8_t dir = s.lastDir;
  portEXIT_CRITICAL(&encMux);
  uint32_t since = micros() - last;
  if (period == 0 || since > 500000) return 0;  // parada (o menos de 1 pulso cada 0.5 s)
  // Si ya pasó más tiempo que el último periodo, la rueda va frenando:
  // usar ese tiempo como cota hace que la velocidad baje sin esperar al siguiente pulso.
  if (since > period) period = since;
  return dir * 1e6f / (period * ticksPerRev_);
}

void Encoders::setCommandSign(Side side, int8_t sign) {
  if (sign != 0) st_[side].cmdSign = sign;
}

void Encoders::reset() {
  portENTER_CRITICAL(&encMux);
  st_[0].count = st_[1].count = 0;
  portEXIT_CRITICAL(&encMux);
}
