// Servidor web + WebSocket.
//
// Mensajes de la página -> ESP32 (JSON, campo "t"):
//   {"t":"cmd","v":0.5,"w":-0.2}   v, w normalizados -1..1 (enviar cada 100 ms)
//   {"t":"stop"}                   paro inmediato
//   {"t":"resetOdom"}              x = y = θ = 0
//   {"t":"getConfig"}
//   {"t":"setConfig","cfg":{...}}  guarda en NVS y reinicia
//   {"t":"pid","en":true,"kp":..,"ki":..,"kd":..,"save":false}   ajuste en vivo
//   {"t":"imuCal"}                 recalibra el giroscopio (robot quieto)
//   {"t":"factoryReset"}           borra NVS y reinicia con valores por defecto
//
// ESP32 -> página:
//   {"t":"config",...}  {"t":"tel",...}  {"t":"msg","level":"ok|warn|err","text":"..."}
#pragma once
#include <ESPAsyncWebServer.h>
#include "Config.h"

// Lo escribe el WebSocket (otra tarea) y lo lee loop().
struct RemoteState {
  volatile float v = 0, w = 0;
  volatile uint32_t lastCmdMs = 0;
  volatile bool hasCmd = false;
  volatile bool stopNow = false;
  volatile bool resetOdom = false;
  volatile bool imuCal = false;
  volatile bool factoryReset = false;
  volatile bool sendConfig = false;
  // Cambios que se aplican desde loop()
  volatile bool pendingCfg = false;
  Config newCfg;
  volatile bool pendingPid = false;
  bool pidEn = true, pidSave = false;
  float kp = 0, ki = 0, kd = 0;
};

class WebUi {
 public:
  WebUi() : server_(80), ws_("/ws") {}
  void begin(Config& cfg, bool configured, RemoteState& remote);
  void loop();
  bool hasClients() { return ws_.count() > 0; }
  void broadcast(const String& json);
  void message(const char* level, const String& text);
  String configJson();

 private:
  void onWsEvent(AsyncWebSocketClient* c, AwsEventType type, void* arg, uint8_t* data, size_t len);
  void handleText(AsyncWebSocketClient* c, const char* data, size_t len);

  AsyncWebServer server_;
  AsyncWebSocket ws_;
  Config* cfg_ = nullptr;
  bool configured_ = false;
  RemoteState* remote_ = nullptr;
  uint32_t lastCleanup_ = 0;
};
