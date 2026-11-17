# ROmBA: control web con ESP32

Proyecto independiente del firmware micro-ROS (`firmware/esp32`) y de ROS 2: aquí
**solo se usa la ESP32**. La ESP32 crea su propia red WiFi, sirve una página web y
recibe los comandos de movimiento por **WebSocket**. Calcula la odometría con los
encoders (y opcionalmente la IMU) y la manda a la página en tiempo real.

Todo lo que depende de los componentes (pines, tipo de encoder y sus PPR, tipo de
IMU, medidas del robot, PID, red WiFi) **se configura desde la página** y se
guarda en la memoria NVS de la ESP32. No hace falta recompilar para cambiarlo.

## Uso rápido

```bash
cd esp32-web-control
pio run -e esp32dev -t upload -t monitor   # ESP32 DevKit V1
pio run -e esp32s3  -t upload -t monitor   # ESP32-S3 DevKitC-1
```

1. Conéctate a la red WiFi **ROmBA** (clave `romba1234`).
2. Abre **http://192.168.4.1** (o http://romba.local).
3. La primera vez se abre la pestaña **Configuración**: revisa tipo de encoder, PPR,
   IMU y medidas del robot, y pulsa **Guardar y reiniciar**.
4. En **Control** maneja con el joystick, los botones o el teclado (flechas/WASD,
   espacio = paro). El deslizador limita la velocidad.

La página va dentro del firmware: basta con un solo `upload`. Si editas
`web/index.html`, el script `tools/embed_web.py` regenera `src/index_html.h`
solo al compilar.

## Conexiones por defecto

| Señal | DevKit V1 | ESP32-S3 |
|---|---|---|
| ENA (PWM motor izq.) | 25 | 4 |
| IN1 / IN2 | 26 / 27 | 5 / 6 |
| ENB (PWM motor der.) | 14 | 7 |
| IN3 / IN4 | 12 / 13 | 15 / 16 |
| Encoder izq. A / B | 34 / 35 | 17 / 18 |
| Encoder der. A / B | 32 / 33 | 12 / 13 |
| I2C SDA / SCL (IMU) | 21 / 22 | 8 / 9 |

El canal B solo se usa con encoder de cuadratura. Todos los pines se cambian desde
la página.

Cuidado en el **DevKit V1**:
- GPIO 34-39 son solo entrada y **no tienen pull-up interno**. Si tu encoder es de
  colector abierto, pon una resistencia de 10 kΩ a 3.3 V.
- GPIO 12 es un pin de arranque: si al encender está en ALTO, la ESP32 no arranca.
  Con L298N no suele pasar; si te pasa, mueve IN3 a otro pin.
- Los encoders deben dar **3.3 V**. Si dan 5 V, usa un divisor o un convertidor de nivel.

En el **S3** se evitaron los pines de arranque (0, 3, 45, 46), USB (19, 20), flash
(26-32) y PSRAM (33-37).

## Parámetros

Valores de fábrica en `include/defaults.h`. Robot de 300 mm de diámetro, ruedas de
130 mm y 230 mm entre ruedas.

| Grupo | Parámetros |
|---|---|
| Robot | diámetro de rueda, distancia entre ruedas, diámetro del robot, velocidad lineal y angular máx., aceleración máx. |
| Encoders | tipo (simple / cuadratura), PPR de cada tipo, relación de engranes, filtro de rebotes, invertir izq./der. |
| IMU | tipo (ninguna / BNO055 / MPU6050), dirección I2C, usar en odometría, peso de la IMU, invertir yaw |
| Motores | frecuencia y resolución PWM, PWM mínimo (zona muerta), invertir izq./der. |
| PID | activo, Kp, Ki, Kd (también se ajusta en vivo desde Control) |
| Pines | ENA, IN1-IN4, ENB, encoders A/B, SDA/SCL |
| WiFi y control | nombre y clave de la red, canal, paro sin comandos (ms), Hz de telemetría y de control |

Para añadir un parámetro: una línea en `CONFIG_FIELDS` (`src/Config.h`) y otra en
`SCHEMA` (`web/index.html`).

## Cómo funciona

**Encoders** (`src/Encoders.cpp`). Interrupción en el flanco de subida del canal A
(cuenta x1). Los PPR se dan **por vuelta de rueda**; si el encoder está en el eje
del motor, pon la relación de engranes.
- *Cuadratura*: el sentido sale del canal B.
- *Simple*: un solo canal no da el sentido, así que se toma del sentido que se le
  está mandando a ese motor. Si la rueda se mueve sin comando (empujando el robot),
  la odometría lo cuenta mal.

La velocidad se mide con el tiempo entre pulsos, así que funciona aunque el disco
tenga pocas ranuras.

**IMU** (`src/Imu.cpp`), sin librerías externas:
- *BNO055*: modo IMUPLUS (acelerómetro + giroscopio, **sin magnetómetro**, porque
  los motores lo distorsionan). Da yaw, roll y pitch ya fusionados.
- *MPU6050*: integra el giroscopio Z para el yaw. Calcula el offset al arrancar,
  así que **el robot debe estar quieto al encender** (o usa *Calibrar giroscopio*).
  Acepta los clones que responden otro WHO_AM_I.

**Odometría** (`src/Odometry.h`), robot diferencial:

```
m/tick = π · D_rueda / (PPR · relación)
dL, dR = Δticks · m/tick
ds     = (dL + dR) / 2
dθ     = (dR − dL) / distancia_entre_ruedas              solo encoders
dθ     = α · dθ_IMU + (1 − α) · dθ_encoders              con fusión (α = peso de la IMU)
x += ds · cos(θ + dθ/2)    y += ds · sin(θ + dθ/2)    θ += dθ
```

**Control** (`src/main.cpp`, 50 Hz). La página manda `v` y `ω` normalizados cada
100 ms. Se convierten a velocidad por rueda (`v ∓ ω·B/2`), pasan por la rampa de
aceleración y van al PID. El PID suma una prealimentación (velocidad objetivo /
velocidad máxima) y solo corrige la diferencia. Con el PID apagado se manda solo
la prealimentación. Si no llegan comandos en `wsTimeoutMs` (500 ms), el robot frena.

## Primeras pruebas

1. **Con las ruedas en el aire**, empuja el joystick hacia adelante. Si una rueda
   gira al revés, activa *Invertir motor*.
2. Con el robot quieto, gira una rueda a mano hacia adelante: los ticks deben
   **subir**. Si bajan (cuadratura), activa *Invertir encoder*.
3. Ajusta el **PWM mínimo**: con el PID apagado, sube la velocidad poco a poco y
   anota el % al que la rueda empieza a moverse.
4. Comprueba la odometría: avanza 1 m medido con cinta. Si `x` no da 1.00, corrige
   el diámetro de rueda. Luego gira 360° en el sitio; si θ no vuelve a 0, corrige
   la distancia entre ruedas (o usa la IMU).
5. PID: empieza con Ki = 0, sube Kp hasta que la medida siga al objetivo sin
   oscilar, y luego sube Ki para quitar el error que queda.

## Protocolo WebSocket (`ws://192.168.4.1/ws`)

Página → ESP32:

```json
{"t":"cmd","v":0.5,"w":-0.2}          // -1..1, enviar cada 100 ms
{"t":"stop"}
{"t":"resetOdom"}
{"t":"pid","en":true,"kp":60,"ki":250,"kd":0,"save":false}
{"t":"imuCal"}
{"t":"getConfig"}
{"t":"setConfig","cfg":{...}}         // guarda en NVS y reinicia
{"t":"factoryReset"}
```

ESP32 → página: `{"t":"config",...}` al conectar (configuración y valores
derivados: ticks/vuelta, mm/tick...), `{"t":"tel",...}` a 10 Hz (x, y, θ, v, ω,
distancia, objetivo/medida/PWM/ticks por rueda, datos de la IMU) y
`{"t":"msg","level":"ok|warn|err","text":"..."}`.

## Estructura

```
esp32-web-control/
├── platformio.ini        entornos esp32dev y esp32s3
├── include/defaults.h    valores de fábrica y pines por placa
├── src/
│   ├── main.cpp          WiFi, lazo de control, telemetría
│   ├── Config.*          parámetros + NVS + validación
│   ├── Motors.*          puente H (ENA/ENB PWM, IN1-IN4)
│   ├── Encoders.*        simple y cuadratura por interrupción
│   ├── Imu.*             BNO055 y MPU6050
│   ├── Odometry.h  Pid.h
│   ├── WebUi.*           servidor HTTP + WebSocket
│   └── index_html.h      generado desde web/index.html
├── web/index.html        interfaz (joystick, botones, trayectoria, configuración)
└── tools/embed_web.py
```
