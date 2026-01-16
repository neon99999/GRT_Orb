# The ORB v2 — ESP32 sACN IRGB

## For Lighting Operator

### Eos setup (Element or any Eos-family)

**Unicast (preferred, Eos v3.2+)**

1. Set the console NIC to static
   IP `10.10.50.2` • Mask `255.255.255.0` • Gateway `10.10.50.1`
2. Settings → sACN → enable on that NIC
3. Patch → Protocols → sACN → Per-Universe Overrides

   * Universe 1 → Add Unicast target `10.10.50.201`
4. Patch channels at **U1@321..325**

   * 321 Intensity
   * 322 Red
   * 323 Green
   * 324 Blue
   * 325 White
     Tip: make a 5-channel fixture profile with I R G B W

**Multicast (older Eos)**

1. In firmware this unit can run multicast. If needed, have the tech switch it
2. Enable sACN multicast on Universe 1 in Eos
3. Patch the same **U1@321..325**

### Channel map (Universe 1)

* **321** = Global Intensity
* **322..396** = Per-pixel RGB, 25 pixels total

  * P1 R=325 G=326 B=327
  * P2 R=328 G=329 B=330
  * …
  * P25 R=394 G=395 B=396
* **Broadcast override**

  * **397** = R, **398** = G, **399** = B
  * If any of these is >0 the whole strip takes that RGB, scaled by 321

That’s it. Bring up 321, set color with 322–325, or use 397–399 to flood all pixels.

---

## Technical Wiring

### Hardware

* ESP32 dev board
* WS2812B strip (5 V) or NeoPixel GRB/GRBW rings
* 330 Ω on DIN
* 1000 µF across +5 and GND at the strip
* Optional level shifter if your run is long
* Power: 5 V USB-C bank that can supply 2–3 A

**Mirror mode**: drive two rings by splitting the same data line after the 330 Ω, each ring has its own 1000 µF, power is star-wired from 5 V.

### Router

* GL-SFT1200 (or similar)
* LAN IP `10.10.50.1`
* DHCP pool `10.10.50.100–10.10.50.150`
* 2.4 GHz only, 20 MHz width, visible SSID, WPA2-PSK AES
* Fixed channel 1 or 6 or 11
* No internet required

### Unit IP

* ESP32 static IP `10.10.50.201/24`
* Gateway `10.10.50.1`
* DNS `10.10.50.1` or `0.0.0.0`
  Console can be `10.10.50.2/24`

### Firmware knobs (in `config.cpp`)

* `USE_UNICAST = true` for Eos unicast, false for multicast
* `E131_UNIVERSE = 1`
* `START_ADDR = 321`
* `NUM_PIXELS = 25`
* Brightness cap: `DEFAULT_BRIGHTNESS_CAP = 180` (helps headroom)
* Idle handling:

  * `IDLE_TIMEOUT_MS = 3000`
  * If you must keep banks awake, choose one:

    * **Software**: set a very faint steady floor with `MIN_IDLE_MASTER = 2..6`
      This is visible, tape over one end pixel if needed
    * **Hardware**: add a 220 Ω 0.5 W across +5 and GND at the strip for ~23 mA with no light

### TouchDesigner testing

* DMX Out CHOP → Interface sACN
* Universe 1
* Unicast to `10.10.50.201` or broadcast if testing multicast
* Feed 399 channels with a CHOP

  * Leave channels 1..320 empty
  * Fill 321..399 as per map above
* Rate ≤ 44 Hz

### Build

* PlatformIO → env `esp32dev` (or your board)
* `lib_deps`: Adafruit NeoPixel, ESPAsyncE131
* Upload, open Serial at 115200

  * On boot you’ll see IP and a short status log

### Power notes

* A single 25-LED strip full white can draw up to 25 × 60 mA ≈ 1.5 A at the LEDs
* With cap 180 and typical looks you’ll be well under that
* Do not feed LED power through the ESP32 Vin trace
* Keep 5 V and GND short and fat, cap at the strip pads

### Troubleshooting

* No response: ping `10.10.50.201`, check Universe 1 enabled, addresses 321..325
* Colors wrong: set `PIXEL_ORDER` to match your LEDs (GRB is common)
* Blanking near full: keep cap at the end of the strip, shorten 5 V leads, brightness cap 180–200
* Bank sleep during blackout:

  * Use the hardware dummy load (220 Ω) for zero-light keep-alive
  * Or a taped pixel with tiny pulse if that is acceptable
* Multicast noise in heavy RF: switch to unicast and a fixed clean channel

---

## Quick reference

* Universe: **1**
* Start address: **321**
* Global Intensity: **321**
* Per-pixel: **P1 325..327**, **P25 394..396**
* Broadcast RGB: **397..399**
* ESP32 IP: **10.10.50.201**