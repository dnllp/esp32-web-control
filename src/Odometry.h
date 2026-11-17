// Odometría de un robot diferencial.
//
//   dL, dR = ticks * (pi * D_rueda / ticks_por_vuelta)
//   ds     = (dR + dL) / 2
//   dθ     = (dR - dL) / distancia_entre_ruedas          (solo encoders)
//   dθ     = α·dθ_imu + (1-α)·dθ_encoders                (con fusión IMU)
//   x += ds·cos(θ + dθ/2)     y += ds·sin(θ + dθ/2)     θ += dθ
#pragma once
#include <math.h>

class Odometry {
 public:
  float x = 0, y = 0, theta = 0;  // m, m, rad (-pi..pi)
  float v = 0, w = 0;             // m/s, rad/s
  float dist = 0;                 // distancia total recorrida (m)

  void reset() {
    x = y = theta = v = w = dist = 0;
  }

  void update(float dL, float dR, float wheelBase, float dt, bool useImu, float imuDTheta,
              float alpha) {
    float ds = (dL + dR) * 0.5f;
    float dth = (dR - dL) / wheelBase;
    if (useImu) dth = alpha * imuDTheta + (1.0f - alpha) * dth;
    x += ds * cosf(theta + dth * 0.5f);
    y += ds * sinf(theta + dth * 0.5f);
    theta = atan2f(sinf(theta + dth), cosf(theta + dth));
    dist += fabsf(ds);
    if (dt > 0) {
      // filtro pasa bajas suave para que la telemetría no brinque
      v = 0.7f * v + 0.3f * (ds / dt);
      w = 0.7f * w + 0.3f * (dth / dt);
    }
  }
};
