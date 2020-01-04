# LED_MQTT
ESP8266 and WS2812B LED strip

# libraries used:
This project is using libraries that are listed below and ESP8266 Arduino core.

ESP8166 Arduino core --> [https://github.com/esp8266/Arduino](https://github.com/esp8266/Arduino)

1. FastLED.h --> [https://github.com/FastLED/FastLED](https://github.com/FastLED/FastLED)
2. DNSServer.h --> [https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer](https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer)
3. ESP8266WiFi.h --> [https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)
4. WiFiManager.h --> [https://github.com/tzapu/WiFiManager](https://github.com/tzapu/WiFiManager)
5. MQTT.h --> [https://github.com/i-n-g-o/esp-mqtt-arduino](https://github.com/i-n-g-o/esp-mqtt-arduino)

# To do list:
- [ ] OTA
- [ ] Array command 
- [ ] More effects
  - [ ] Rainbow (steady, moving-animation)
- [ ] PCB design for ESP with temperature safety of power supply
- [ ] Tuch switch for the room (ESP32 or something like that)
- [ ] Test to see if there is a need to run LEDs on max(255)
- [ ] In INO file we need to split effect in a new tab (easier to read)
- [ ] Start working on Raspberry broker that will be used locally
- [ ] Mobile app
