#include "WebUi.h"
#include "index_html.h"

static const char* FW_VERSION = "1.0.0";

void WebUi::begin(Config& cfg, bool configured, RemoteState& remote) {
  cfg_ = &cfg;
  configured_ = configured;
  remote_ = &remote;

  ws_.onEvent([this](AsyncWebSocket*, AsyncWebSocketClient* c, AwsEventType t, void* arg,
                     uint8_t* data, size_t len) { onWsEvent(c, t, arg, data, len); });
  server_.addHandler(&ws_);

  server_.on("/", HTTP_GET, [](AsyncWebServerRequest* req) {
    AsyncWebServerResponse* r =
        req->beginResponse(200, "text/html; charset=utf-8", (const uint8_t*)INDEX_HTML, strlen(INDEX_HTML));
    r->addHeader("Cache-Control", "no-cache");
    req->send(r);
  });
  server_.on("/api/config", HTTP_GET, [this](AsyncWebServerRequest* req) {
    req->send(200, "application/json", configJson());
  });
  server_.onNotFound([](AsyncWebServerRequest* req) { req->redirect("/"); });
  server_.begin();
}

void WebUi::loop() {
  if (millis() - lastCleanup_ > 1000) {
    lastCleanup_ = millis();
    ws_.cleanupClients();
  }
}

void WebUi::broadcast(const String& json) {
  if (ws_.count() > 0 && ws_.availableForWriteAll()) ws_.textAll(json);
}

void WebUi::message(const char* level, const String& text) {
  JsonDocument d;
  d["t"] = "msg";
  d["level"] = level;
  d["text"] = text;
  String s;
  serializeJson(d, s);
  ws_.textAll(s);
}

String WebUi::configJson() {
  const Config& c = *cfg_;
  JsonDocument d;
  d["t"] = "config";
  d["board"] = BOARD_NAME;
  d["fw"] = FW_VERSION;
  d["configured"] = configured_;
  c.toJson(d["cfg"].to<JsonObject>());
  JsonObject der = d["derived"].to<JsonObject>();
  der["ppr"] = c.ppr();
  der["ticksPerRev"] = c.ticksPerRev();
  der["wheelCircM"] = c.wheelCircM();
  der["mmPerTick"] = c.metersPerTick() * 1000.0f;
  der["wheelBaseM"] = c.wheelBaseM();
  // Velocidad máxima de cada rueda cuando se combinan avance y giro
  der["maxWheelSpeed"] = c.maxSpeed + c.maxAngular * c.wheelBaseM() / 2;
  String s;
  serializeJson(d, s);
  return s;
}

void WebUi::onWsEvent(AsyncWebSocketClient* c, AwsEventType type, void* arg, uint8_t* data,
                      size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("[ws] cliente %u conectado\n", c->id());
    c->text(configJson());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[ws] cliente %u desconectado\n", c->id());
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    // Solo mensajes de texto completos en un solo marco (los nuestros son pequeños)
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      handleText(c, (const char*)data, len);
  }
}

void WebUi::handleText(AsyncWebSocketClient* c, const char* data, size_t len) {
  JsonDocument d;
  if (deserializeJson(d, data, len)) return;
  RemoteState& r = *remote_;
  const char* t = d["t"] | "";

  if (!strcmp(t, "cmd")) {
    r.v = constrain(d["v"] | 0.0f, -1.0f, 1.0f);
    r.w = constrain(d["w"] | 0.0f, -1.0f, 1.0f);
    r.lastCmdMs = millis();
    r.hasCmd = true;
  } else if (!strcmp(t, "stop")) {
    r.v = r.w = 0;
    r.stopNow = true;
  } else if (!strcmp(t, "resetOdom")) {
    r.resetOdom = true;
  } else if (!strcmp(t, "imuCal")) {
    r.imuCal = true;
  } else if (!strcmp(t, "getConfig")) {
    c->text(configJson());
  } else if (!strcmp(t, "factoryReset")) {
    r.factoryReset = true;
  } else if (!strcmp(t, "pid")) {
    r.pidEn = d["en"] | cfg_->pidEnabled;
    r.kp = d["kp"] | cfg_->kp;
    r.ki = d["ki"] | cfg_->ki;
    r.kd = d["kd"] | cfg_->kd;
    r.pidSave = d["save"] | false;
    r.pendingPid = true;
  } else if (!strcmp(t, "setConfig")) {
    if (r.pendingCfg) return;
    Config nc = *cfg_;
    String err = nc.fromJson(d["cfg"].as<JsonObjectConst>());
    if (err.length()) {
      JsonDocument m;
      m["t"] = "msg";
      m["level"] = "err";
      m["text"] = err;
      String s;
      serializeJson(m, s);
      c->text(s);
      return;
    }
    r.newCfg = nc;
    r.pendingCfg = true;  // loop() la guarda y reinicia
  }
}
