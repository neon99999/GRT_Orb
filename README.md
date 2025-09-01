# ESP32 sACN IRGBW Ring

## Eos Element setup

**If Eos v3.2+ and you want unicast**

1. Console NIC static: IP 10.10.50.2, mask 255.255.255.0, gateway 10.10.50.1
2. Enable sACN on that NIC
3. Patch → Protocols → sACN → Per-Universe Overrides

   * Universe 1 → add unicast 10.10.50.201
4. Patch at Universe 1 address 321

   * Quick: patch five channels at U1\@321..325 named I R G B W
   * Better: make a 5-channel fixture with parameters Intensity Red Green Blue White

**If your Eos does not have unicast**

1. Set `USE_UNICAST = false` in firmware and reflash
2. Enable sACN multicast for Universe 1
3. Patch as above at U1\@321..325


## Parts

* ESP32 dev board
* 2 Adafruit NeoPixel 12-LED RGBW rings
* SparkFun BSS138 level shifter BOB-12009
* 2 x 330 Ω resistors
* 2 x 1000 µF electrolytic caps (one per ring)
* GL-SFT1200 router
* 5 V USB-C power bank (3A)
* USB A-C Cable
* USB C-C Cable

## Wiring (mirror mode, 2 rings)

1. Star power from 5 V source

   * +5 V → ESP32 Vin, Ring A +5, Ring B +5
   * GND → ESP32 GND, Ring A GND, Ring B GND
2. Level shifter

   * LV → ESP32 3V3, HV → +5 V, GND common
   * ESP32 data GPIO → LV1
   * HV1 → 330 Ω → Ring A DIN
   * HV1 → another 330 Ω → Ring B DIN
3. Caps

   * 1000 µF across +5 and GND on each ring (close to pads)
4. Switch

   * Switch +5 V, never ground

## Firmware

1. Open the PlatformIO project
2. Edit `src/config.cpp`

   ```cpp
   const char WIFI_SSID[]     = "YOUR_SSID";
   const char WIFI_PASSWORD[] = "YOUR_PASSWORD";
   ```
3. Edit `src/config.h`

   ```cpp
   constexpr bool     USE_UNICAST    = true;      // set false if your Eos lacks unicast
   constexpr uint16_t E131_UNIVERSE  = 1;
   constexpr uint16_t START_ADDR     = 321;       // IRGBW at 321..325
   constexpr uint8_t  DEFAULT_BRIGHTNESS_CAP = 155;

   constexpr bool USE_STATIC_IP = true;
   const IPAddress STATIC_IP (10,10,50,201);
   const IPAddress STATIC_GW (10,10,50,1);
   const IPAddress STATIC_SN (255,255,255,0);
   const IPAddress STATIC_DNS(10,10,50,1);
   ```
4. Build and upload
5. On boot the ring blinks the IP and Serial prints the IP and sACN status

## Router (GL-SFT1200)

1. LAN IP 10.10.50.1
2. DHCP pool 10.10.50.100–10.10.50.150
3. 2.4 GHz only, SSID visible, WPA2-PSK AES
4. Channel width 20 MHz, fixed channel 1 (try 6 or 11 if needed)
5. Band steering off, client/AP isolation off


## Use

1. Power router, then ESP32
2. Confirm ESP32 IP shows as 10.10.50.201
3. From console PC, ping 10.10.50.201
4. Bring up Intensity, then color on the console at U1\@321..325
5. For two rings both will mirror

## Power notes

* Two rings at cap 155 draw about 1.3 A total including ESP32
* Use a 3 A-rated USB-C output and short cable
* Keep caps on each ring
* Do not run ring power through the ESP32 board traces

## Troubleshooting

* No light: check Universe 1 active, addresses 321..325, same subnet
* Old Eos: use multicast and set firmware to `USE_UNICAST = false`
* Blacks out at high levels: measure volts across the ring cap, shorten and thicken +5 V and GND, keep cap ≥1000 µF
* Wrong colors: change `PIXEL_ORDER` to `NEO_RGBW` and rebuild
