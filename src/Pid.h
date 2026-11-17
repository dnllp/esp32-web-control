// PID de velocidad por rueda. Entrada: m/s. Salida: % de PWM.
// El lazo suma una prealimentación (velocidad objetivo / velocidad máxima),
// así el PID solo corrige la diferencia y es más fácil de ajustar.
#pragma once

class Pid {
 public:
  float kp = 0, ki = 0, kd = 0;
  float outMin = -100, outMax = 100;

  void reset() {
    integ_ = 0;
    prevMeas_ = 0;
    first_ = true;
  }

  // ff = prealimentación en %, ya incluida en la saturación.
  float update(float target, float meas, float ff, float dt) {
    float err = target - meas;
    // Derivada sobre la medición: evita el salto cuando cambia el objetivo.
    float deriv = first_ ? 0 : -(meas - prevMeas_) / dt;
    first_ = false;
    prevMeas_ = meas;
    float out = ff + kp * err + integ_ + kd * deriv;
    // Anti-windup: solo integra si no está saturado, o si el error lo saca de la saturación.
    bool satHigh = out >= outMax && err > 0;
    bool satLow = out <= outMin && err < 0;
    if (!satHigh && !satLow) integ_ += ki * err * dt;
    out = ff + kp * err + integ_ + kd * deriv;
    if (out > outMax) out = outMax;
    if (out < outMin) out = outMin;
    return out;
  }

 private:
  float integ_ = 0, prevMeas_ = 0;
  bool first_ = true;
};
