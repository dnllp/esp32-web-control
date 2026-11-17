#include "Config.h"
#include <Preferences.h>
#include <driver/gpio.h>

// Cambia este número si cambias el struct Config: así una configuración
// vieja guardada en NVS se descarta en vez de leerse mal.
static const uint32_t CONFIG_VERSION = 1;
static const char* NVS_NS = "romba";

void Config::setDefaults() {
  strlcpy(apSsid, DEF_AP_SSID, sizeof(apSsid));
  strlcpy(apPass, DEF_AP_PASS, sizeof(apPass));
#define X_DEF(type, name, def) name = (type)(def);
  CONFIG_FIELDS(X_DEF)
#undef X_DEF
}

void Config::toJson(JsonObject obj) const {
  obj["apSsid"] = apSsid;
  obj["apPass"] = apPass;
#define X_OUT(type, name, def) obj[#name] = name;
  CONFIG_FIELDS(X_OUT)
#undef X_OUT
}

String Config::fromJson(JsonObjectConst obj) {
  if (obj["apSsid"].is<const char*>()) strlcpy(apSsid, obj["apSsid"], sizeof(apSsid));
  if (obj["apPass"].is<const char*>()) strlcpy(apPass, obj["apPass"], sizeof(apPass));
  // Los checkboxes llegan como bool y los números como número.
#define X_IN(type, name, def)                                   \
  if (!obj[#name].isNull()) name = (type)obj[#name].as<double>();
  CONFIG_FIELDS(X_IN)
#undef X_IN
  return validate();
}

static bool validOut(uint8_t p) { return GPIO_IS_VALID_OUTPUT_GPIO((gpio_num_t)p); }
static bool validIn(uint8_t p) { return GPIO_IS_VALID_GPIO((gpio_num_t)p); }

String Config::validate() const {
  if (strlen(apSsid) == 0) return "El nombre de la red WiFi no puede estar vacío";
  size_t pl = strlen(apPass);
  if (pl > 0 && pl < 8) return "La clave WiFi debe tener al menos 8 caracteres (o dejarla vacía)";
  if (apChannel < 1 || apChannel > 13) return "Canal WiFi entre 1 y 13";

  const uint8_t outs[] = {pinEna, pinIn1, pinIn2, pinEnb, pinIn3, pinIn4, pinSda, pinScl};
  for (uint8_t p : outs)
    if (!validOut(p)) return "GPIO " + String(p) + " no sirve como salida en esta placa";
  const uint8_t ins[] = {pinEncLA, pinEncLB, pinEncRA, pinEncRB};
  for (uint8_t p : ins)
    if (!validIn(p)) return "GPIO " + String(p) + " no existe en esta placa";

  if (wheelDiamMm <= 0 || wheelBaseMm <= 0) return "Diámetro de rueda y distancia entre ruedas deben ser > 0";
  if (pprSimple == 0 || pprQuad == 0 || gearRatio <= 0) return "PPR y relación de engranes deben ser > 0";
  if (pwmBits < 8 || pwmBits > 12) return "Resolución PWM entre 8 y 12 bits";
  if (pwmFreq < 100 || pwmFreq > 20000) return "Frecuencia PWM entre 100 y 20000 Hz";
  if (pwmMinPct < 0 || pwmMinPct >= 100) return "PWM mínimo entre 0 y 99 %";
  if (encType > ENC_QUADRATURE) return "Tipo de encoder inválido";
  if (imuType > IMU_MPU6050) return "Tipo de IMU inválido";
  if (imuAlpha < 0 || imuAlpha > 1) return "Peso de la IMU entre 0 y 1";
  if (maxSpeed <= 0 || maxAngular <= 0) return "Velocidades máximas deben ser > 0";
  if (telemetryHz < 1 || telemetryHz > 50) return "Telemetría entre 1 y 50 Hz";
  if (controlHz < 10 || controlHz > 200) return "Lazo de control entre 10 y 200 Hz";
  if (wsTimeoutMs < 100) return "El tiempo de paro debe ser al menos 100 ms";
  return "";
}

namespace ConfigStore {

bool load(Config& cfg) {
  cfg.setDefaults();
  Preferences p;
  if (!p.begin(NVS_NS, true)) return false;
  bool ok = p.getUInt("ver", 0) == CONFIG_VERSION && p.getBytesLength("cfg") == sizeof(Config);
  if (ok) {
    Config tmp;
    p.getBytes("cfg", &tmp, sizeof(Config));
    tmp.apSsid[sizeof(tmp.apSsid) - 1] = 0;
    tmp.apPass[sizeof(tmp.apPass) - 1] = 0;
    if (tmp.validate().length() == 0) cfg = tmp;
    else ok = false;
  }
  p.end();
  return ok;
}

bool save(const Config& cfg) {
  Preferences p;
  if (!p.begin(NVS_NS, false)) return false;
  bool ok = p.putBytes("cfg", &cfg, sizeof(Config)) == sizeof(Config);
  ok = ok && p.putUInt("ver", CONFIG_VERSION) > 0;
  p.end();
  return ok;
}

void erase() {
  Preferences p;
  if (p.begin(NVS_NS, false)) {
    p.clear();
    p.end();
  }
}

}  // namespace ConfigStore
