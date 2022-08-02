
# Esp32-Smart-Light
## Smart Light Control

This device for created as alternative for solutions based on bi-stable relays to control light in house with posibility to control light remotely and some features that exist in SONOFF BASIC devices.
 
Solution is implemented on [ESP-WROOM-32 38 PIN Develeopment Board PINOUT](https://www.studiopieters.nl/esp32-pinout/). 38 pin  edition was selected to maximize number of PIN. Minimal requirement was to have at least 8 buttons and 8 relays.
![PIN-OUT](https://raw.githubusercontent.com/AchimPieters/esp32-homekit-camera/master/Images/ESP32-38%20PIN-DEVBOARD.png)	

## Used references

To dynamically configure WiFi used [Wi-Fi Manager Library](https://github.com/tzapu/WiFiManager). I have tried to investigate but had no luck to discover what approach/Library is used in WiFiManager to Store Credentials. I only found that EEPROM lib used earlier.


For storing device configuration permanently in device used [Preferences Library](https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/preferences.html).
[GiHub](https://github.com/vshymanskyy/Preferences)


For JSON manipulation, to parse or deserialize complex or nested JSON objects and arrays used FirebaseJson library. Library was set as part of Firebase ESP32 Client library.
[GiHub](https://github.com/mobizt/FirebaseJson)

Arduino Timezone Library
https://github.com/JChristensen/Timezone


Firebase Realtime Database Arduino Library for ESP32
https://github.com/mobizt/Firebase-ESP32
