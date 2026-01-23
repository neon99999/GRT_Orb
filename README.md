Here is the **cleaned and updated README** with **all fixed-address language removed** and replaced by **“configurable via the web interface”**. No hard numbers are implied anywhere except in examples.

---

# The ORB v3 — ESP32 sACN IRGB

## For Lighting Operator

### Eos setup (Element or any Eos-family)

**Unicast (preferred, Eos v3.2+)**

1. Set the console NIC to static
   IP `10.10.50.2` • Mask `255.255.255.0` • Gateway `10.10.50.1`
2. Settings → sACN → enable on that NIC
3. Patch → Protocols → sACN → Per-Universe Overrides
4. Add a unicast target using the IP shown in the ORB web interface

**Multicast (older Eos)**

1. This unit can operate in multicast if configured
2. Enable sACN multicast for the selected universe in Eos

---

## Channel map (per configured universe)

> The universe, start address, and pixel count are all configurable in the ORB web interface.

### Per-pixel IRGB mode

* **Start address** = Global Intensity
* **Following channels** = Per-pixel RGB in order
* Pixel 1 maps to the **second physical LED**

Example layout shown for clarity only:

* Intensity
* P1 R G B
* P2 R G B
* …
* PN R G B

### Broadcast override (optional)

* Immediately follows the per-pixel block
* Three channels: R G B
* If any channel is >0, the entire strip takes that RGB color scaled by the global intensity

---

## Reserved always-on pixel

* The **first physical LED** is reserved by firmware
* It is always on at a very low level
* It is **not addressable** from DMX
* Purpose:

  * Keeps USB power banks awake
  * Provides a visual heartbeat
  * Intended to be taped over

Pixel count configured in the web interface refers only to **DMX-addressable pixels**.

---

## Web interface

The ORB includes a built-in configuration UI.

### Access

* `http://<unit-ip>`
* or `http://orb.local` if mDNS is available

### Configurable parameters (live)

Changes apply immediately and persist across reboots.

* sACN universe
* DMX start address
* Run mode

  * 4-channel IRGB
  * Per-pixel IRGB
* DMX-addressable pixel count
* Brightness cap
* Unicast enable or disable

Changing the pixel count safely reinitializes the LED driver without rebooting.

---

## Technical wiring

### Hardware

* ESP32 dev board
* WS2812B or NeoPixel-compatible LEDs (5 V)
* 330 Ω resistor on DIN
* 1000 µF capacitor across +5 V and GND at the strip
* Optional level shifter for long data runs
* Power: 5 V USB-C supply or bank capable of 2–3 A

**Mirror mode**: split the data line after the resistor to drive two identical strips. Each strip requires its own capacitor. Power is star-wired.

---

## Network

### Router

* Small travel router or access point
* 2.4 GHz only
* 20 MHz channel width
* Fixed channel (1, 6, or 11)
* WPA2-PSK AES
* No internet required

### Unit IP

* Static or DHCP, depending on configuration
* IP address is shown in the web interface and on boot over Serial

---

## TouchDesigner testing

* DMX Out CHOP → Interface sACN
* Match the universe and addressing shown in the web UI
* Unicast to the unit IP or use multicast
* Feed enough channels to cover intensity, pixels, and optional broadcast
* Recommended rate ≤ 44 Hz

---

## Build

* PlatformIO using an ESP32 target
* Libraries:

  * Adafruit NeoPixel
  * ESPAsyncE131
  * ESPAsyncWebServer
* Upload firmware
* Open Serial at 115200 to view boot status and IP

---

## Power notes

* Each RGB LED can draw up to ~60 mA at full white
* Brightness cap limits peak current
* Do not power LEDs through the ESP32 Vin trace
* Keep 5 V and GND short and thick
* Always place the capacitor at the LED pads

---

## Troubleshooting

* No output: confirm universe and addressing in the web interface
* Wrong colors: verify pixel color order in firmware
* Flicker or brownout: shorten power leads, confirm voltage at strip, lower brightness cap
* Power bank sleeping: reserved always-on pixel keeps most banks awake
* RF instability: use unicast and a fixed Wi-Fi channel

---

## Quick reference

* Universe: configurable via web UI
* Start address: configurable via web UI
* Pixel count: configurable via web UI
* First physical LED: reserved, always on
* Configuration UI: `http://<unit-ip>` or `http://orb.local`
