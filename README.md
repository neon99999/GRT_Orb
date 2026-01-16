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


## Pixel Mapping

321 = Global Intensity
322..396 = per-pixel RGB (25 × 3)
397 = Broadcast R, 398 = Broadcast G, 399 = Broadcast B

### Per-pixel block (U1)
P1 R=322 G=323 B=324
P2 R=325 G=326 B=327
P3 R=328 G=329 B=330
P4 R=331 G=332 B=333
P5 R=334 G=335 B=336
P6 R=337 G=338 B=339
P7 R=340 G=341 B=342
P8 R=343 G=344 B=345
P9 R=346 G=347 B=348
P10 R=349 G=350 B=351
P11 R=352 G=353 B=354
P12 R=355 G=356 B=357
P13 R=358 G=359 B=360
P14 R=361 G=362 B=363
P15 R=364 G=365 B=366
P16 R=367 G=368 B=369
P17 R=370 G=371 B=372
P18 R=373 G=374 B=375
P19 R=376 G=377 B=378
P20 R=379 G=380 B=381
P21 R=382 G=383 B=384
P22 R=385 G=386 B=387
P23 R=388 G=389 B=390
P24 R=391 G=392 B=393
P25 R=394 G=395 B=396

### Broadcast override (U1)
R=397 G=398 B=399
If any of these three > 0, all pixels show that color, multiplied by channel 321.


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
