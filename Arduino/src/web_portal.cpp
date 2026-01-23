#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <ESPAsyncWebServer.h>
#include "config.h"
#include "leds.h"

static Preferences prefs;
static AsyncWebServer server(80);

static void loadAll()
{
    // load with current values as defaults
    E131_UNIVERSE = prefs.getUShort("universe", E131_UNIVERSE);
    START_ADDR = prefs.getUShort("start", START_ADDR);
    NUM_PIXELS = prefs.getInt("pixels", NUM_PIXELS);
    DEFAULT_BRIGHTNESS_CAP = prefs.getUChar("cap", DEFAULT_BRIGHTNESS_CAP);
    USE_UNICAST = prefs.getBool("unicast", USE_UNICAST);
    RUN_MODE = prefs.getUChar("mode", RUN_MODE);
}

static const char *INDEX_HTML = R"HTML(
<!doctype html><meta charset=utf-8><meta name=viewport content="width=device-width,initial-scale=1">
<title>ORB Config</title>
<style>body{font-family:system-ui;margin:0;background:#111;color:#eee}
main{max-width:720px;margin:24px auto;padding:16px}
.card{background:#1b1b1b;border-radius:12px;padding:16px;margin:16px 0;border:1px solid #2a2a2a}
label{display:block;margin:8px 0 4px;color:#bbb}input,select{width:100%;padding:8px;border-radius:8px;border:1px solid #333;background:#0f0f0f;color:#eee}
.row{display:grid;grid-template-columns:1fr 1fr;gap:12px}button{padding:10px 14px;border:1px solid #2a2a2a;background:#2b2b2b;color:#fff;border-radius:10px}
</style>
<main>
<h1>ORB Config</h1>
<div class=card>
<form id=f>
<div class=row>
  <div><label>Universe</label><input id=universe type=number min=1 max=63999></div>
  <div><label>Start address</label><input id=start type=number min=1 max=512></div>
</div>
<div class=row>
  <div><label>Mode</label>
    <select id=mode>
      <option value=0>4-channel (I,R,G,B)</option>
      <option value=1>Per-pixel (I + N×RGB)</option>
    </select>
  </div>
  <div><label>Pixels (per-pixel)</label><input id=pixels type=number min=1 max=512></div>
</div>
<div class=row>
  <div><label>Brightness cap</label><input id=cap type=number min=0 max=255></div>
  <div><label>Unicast</label>
    <select id=unicast><option value=1>True</option><option value=0>False</option></select>
  </div>
</div>
<p><button type=submit>Save</button></p>
</form>
</div>
<div class=card><pre id=stat>Loading...</pre></div>
<script>
async function load(){
  const c = await (await fetch('/config.json')).json();
  universe.value=c.universe; start.value=c.start; mode.value=c.mode;
  pixels.value=c.pixels; cap.value=c.cap; unicast.value=c.unicast?1:0;
  const s = await (await fetch('/status')).json();
  stat.textContent = JSON.stringify(s,null,2);
}
f.onsubmit=async(e)=>{e.preventDefault();
  const body = {
    universe:+universe.value, start:+start.value, mode:+mode.value,
    pixels:+pixels.value, cap:+cap.value, unicast:unicast.value=='1'
  };
  await fetch('/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(body)});
  await load();
};
load();
</script>
</main>
)HTML";

void webPortalBegin()
{
    prefs.begin("orb", false);
    loadAll();
    MDNS.begin("orb");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *r)
              { r->send(200, "text/html", INDEX_HTML); });

    server.on("/config.json", HTTP_GET, [](AsyncWebServerRequest *r)
              {
    String out="{";
    out += "\"universe\":" + String(E131_UNIVERSE);
    out += ",\"start\":"    + String(START_ADDR);
    out += ",\"mode\":"     + String(RUN_MODE);
    out += ",\"pixels\":"   + String(NUM_PIXELS);
    out += ",\"cap\":"      + String(DEFAULT_BRIGHTNESS_CAP);
    out += ",\"unicast\":"  + String(USE_UNICAST ? "true":"false");
    out += "}";
    r->send(200, "application/json", out); });

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *r)
              {
    String out="{";
    out += "\"ip\":\"" + WiFi.localIP().toString() + "\"";
    out += ",\"rssi\":" + String(WiFi.RSSI());
    out += ",\"heap\":" + String(ESP.getFreeHeap());
    out += "}";
    r->send(200, "application/json", out); });

    server.on("/config", HTTP_POST, [](AsyncWebServerRequest *r) {}, NULL, [](AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t, size_t)
              {
      String s((const char*)data, len);
      auto num=[&](const char* k,long d)->long{
        String key="\""; key+=k; key+="\":"; int i=s.indexOf(key);
        if (i<0) return d; i+=key.length();
        return s.substring(i).toInt();
      };
      auto bln=[&](const char* k,int d)->int{
        String key="\""; key+=k; key+="\":"; int i=s.indexOf(key);
        if (i<0) return d; i+=key.length();
        if (s.substring(i,i+4)=="true") return 1;
        if (s.substring(i,i+5)=="false") return 0;
        return d;
      };

      uint16_t universe = (uint16_t)max(1L,           num("universe", E131_UNIVERSE));
      uint16_t start    = (uint16_t)max(1L, min(512L, num("start",    START_ADDR)));
      uint8_t  mode     = (uint8_t) constrain(        num("mode",     RUN_MODE), 0, 1);
      int      pixels   = (int)     max(1L,           num("pixels",   NUM_PIXELS));
      uint8_t  cap      = (uint8_t)constrain(         num("cap",      DEFAULT_BRIGHTNESS_CAP), 0, 255);
      bool     unicast  = bln("unicast", USE_UNICAST?1:0)==1;

      bool changed=false;
      if (universe != E131_UNIVERSE){ E131_UNIVERSE = universe; prefs.putUShort("universe", E131_UNIVERSE); changed=true; }
      if (start    != START_ADDR   ){ START_ADDR    = start;    prefs.putUShort("start",    START_ADDR);    changed=true; }
      if (mode     != RUN_MODE     ){ RUN_MODE      = mode;     prefs.putUChar ("mode",      RUN_MODE);      changed=true; }
      if (pixels   != NUM_PIXELS   ){ NUM_PIXELS    = pixels;   prefs.putInt   ("pixels",    NUM_PIXELS);    ledsInit(); changed=true; }
      if (cap      != DEFAULT_BRIGHTNESS_CAP){ DEFAULT_BRIGHTNESS_CAP = cap; prefs.putUChar("cap", cap); ledsSetBrightnessCap(cap); changed=true; }
      if (unicast  != USE_UNICAST  ){ USE_UNICAST   = unicast;  prefs.putBool  ("unicast",   USE_UNICAST);   changed=true; }

      r->send(200, "application/json", String("{\"ok\":true,\"changed\":") + (changed?"true}":"false}")); });

    server.begin();
}
