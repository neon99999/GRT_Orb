# The ORB ESP32 sACN IRGB

## For Lighting Operator

### Eos setup (Element or any Eos-family)

**Unicast (preferred, Eos v3.2+)**

1. Set the console NIC to static IP `10.10.50.2` • Mask `255.255.255.0` • Gateway `10.10.50.1`
2. Settings → sACN → enable on that NIC
3. Patch → Protocols → sACN → Per-Universe Overrides
4. Add a unicast target using the ORB IP

**Multicast (older Eos)**

1. This unit can operate in multicast if configured
2. Enable sACN multicast for the selected universe in Eos

---

## Channel map (per configured universe)

> The universe, start address and pixel count are all configurable in the ORB web interface.

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

  * Helps keep USB power banks awake
  * Provides a simple powered-state indicator
  * Intended to be taped over

Pixel count configured in the web interface refers only to **DMX-addressable pixels**.

For example, if there are 24 physical LEDs and the first is reserved, configure 23 DMX-addressable pixels.

---

## Web interface

The ORB includes a built-in configuration UI.

### Access

* `http://10.10.50.201`
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

**Mirror mode**: split the data line after the resistor to drive two identical strips. Each strip requires its own 1000 µF capacitor. Power is star-wired.

---

## Network

### Required network settings

The ORB firmware expects the following network configuration:

* Wi-Fi SSID: `ORB`
* Wi-Fi password: configured in firmware and on the router
* Router LAN IP: `10.10.50.1`
* Subnet mask: `255.255.255.0`
* ORB static IP: `10.10.50.201`
* ORB gateway: `10.10.50.1`
* ORB DNS: `10.10.50.1`

The router does not require an internet connection.

---

## Replacement router setup

Almost any standard Wi-Fi router or travel router can be used as long as it supports a 2.4 GHz Wi-Fi network and allows its LAN address to be changed.

### 1. Connect to the router

* Power on the router
* Connect a laptop using Ethernet or the router's default Wi-Fi network
* Open the router's administration page

The default address and login procedure vary by manufacturer. Check the router label or manual if necessary.

### 2. Configure the LAN

Set:

* Router IP: `10.10.50.1`
* Subnet mask: `255.255.255.0`

Save the settings.

The router may reboot. Afterward, reconnect using:

`http://10.10.50.1`

### 3. Configure DHCP

Set the DHCP range to something similar to:

* Start: `10.10.50.100`
* End: `10.10.50.150`

Do not include `10.10.50.201` in the DHCP pool because that address is reserved for the ORB.

Also keep `10.10.50.2` outside the DHCP pool if it will be used as the lighting console address.

### 4. Configure Wi-Fi

Configure the **2.4 GHz** network with:

* SSID: `ORB`
* Password: must match `WIFI_PASSWORD` in the ORB firmware
* Security: WPA2-Personal / WPA2-PSK AES
* Channel width: 20 MHz
* Channel: 1, 6 or 11
* SSID broadcast: enabled

The SSID is case-sensitive and must be exactly:

`ORB`

### 5. 5 GHz settings

5 GHz can remain enabled for other devices, but the ORB requires a compatible 2.4 GHz network.

If the router combines 2.4 GHz and 5 GHz under one SSID and the ORB has connection problems, separate the bands and use `ORB` for the 2.4 GHz network.

### 6. Connect the ORB

After configuring the router:

1. Power-cycle the ORB
2. Allow several seconds for Wi-Fi connection
3. Connect a laptop to the `ORB` network
4. Open:

`http://10.10.50.201`

If the web interface loads, the ORB is connected correctly.

You can also test from a computer with:

```text
ping 10.10.50.201
```

### 7. Connect an Eos console

Connect the console Ethernet port to a **LAN** port on the router.

Suggested console network settings:

* IP: `10.10.50.2`
* Subnet mask: `255.255.255.0`
* Gateway: `10.10.50.1`

Do not connect the console to the router's WAN or Internet port.

### 8. Configure the ORB

Open the ORB web interface and configure:

* sACN universe
* DMX start address
* pixel count
* operating mode
* brightness cap
* unicast or multicast

DMX addressing does not need to be configured in the router.

---

## TouchDesigner testing

* DMX Out CHOP → Interface sACN
* Match the universe and addressing shown in the web UI
* Unicast to the ORB IP or use multicast
* Feed enough channels to cover intensity, pixels and optional broadcast
* Recommended rate ≤ 44 Hz

---

## Build

* PlatformIO using an ESP32 target
* Libraries:

  * Adafruit NeoPixel
  * ESPAsyncE131
  * ESPAsyncWebServer
  * AsyncTCP
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

* No web interface: confirm the device is connected to the `ORB` 2.4 GHz network
* Cannot ping the ORB: confirm the router LAN is `10.10.50.1/24`
* No DMX output: confirm universe and addressing in the web interface
* Wrong colors: verify pixel color order in firmware
* Flicker or brownout: shorten power leads, confirm voltage at the strip and lower brightness cap
* Power bank sleeping: confirm the reserved always-on pixel is illuminated
* RF instability: use unicast, 20 MHz channel width and a fixed Wi-Fi channel
* ORB will not join Wi-Fi: confirm the SSID is exactly `ORB`, verify the password matches the firmware and confirm WPA2 is enabled

---

## Quick reference

* Wi-Fi SSID: `ORB`
* Router IP: `10.10.50.1`
* ORB IP: `10.10.50.201`
* Console IP: `10.10.50.2`
* Universe: configurable via web UI
* Start address: configurable via web UI
* Pixel count: configurable via web UI
* First physical LED: reserved, always on
* Configuration UI: `http://10.10.50.201` or `http://orb.local`
