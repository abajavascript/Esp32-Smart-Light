
# Esp32-Smart-Light
## Smart Light Control

This device for created as alternative for solutions based on bi-stable relays to control light in house with posibility to control light remotely and some features that exist in SONOFF BASIC devices.
 
Solution is implemented on [ESP-WROOM-32 38 PIN Develeopment Board PINOUT](https://www.studiopieters.nl/esp32-pinout/). 38 pin  edition was selected to maximize number of PIN. Minimal requirement was to have at least 8 buttons and 8 relays.
![PIN-OUT](https://raw.githubusercontent.com/AchimPieters/esp32-homekit-camera/master/Images/ESP32-38%20PIN-DEVBOARD.png)	

## Used references

To dynamically configure WiFi used [Wi-Fi Manager Library](https://github.com/tzapu/WiFiManager). I have tried to investigate but had no luck to discover what approach/Library is used in WiFiManager to Store Credentials. I only found that EEPROM lib used earlier.


For storing device configuration permanently in device used [Preferences Library](https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/preferences.html).
[GiHub](https://github.com/vshymanskyy/Preferences)

### JSON Library
For device configuration: Butoon PIN numbers, Relay PIN Numbers, Relay groups, Button Actions etc used JSON format. At first considered to use [ArduinoJSON library](https://arduinojson.org/). However, for JSON manipulation, to parse or deserialize complex or nested JSON objects and arrays selected [FirebaseJson library](https://github.com/mobizt/FirebaseJson), as it is a part of already used [Firebase ESP32](https://github.com/mobizt/Firebase-ESP32) library.


Arduino Timezone Library
https://github.com/JChristensen/Timezone


Article [Network Time Protocol (NTP), Timezone and Daylight saving time (DST) with esp8266, esp32 or Arduino](https://www.mischianti.org/2020/08/08/network-time-protocol-ntp-timezone-and-daylight-saving-time-dst-with-esp8266-esp32-or-arduino/)
Library [NTPClient](https://github.com/arduino-libraries/NTPClient)
Article [Button (external) interrupts with ESP32](https://iotespresso.com/button-external-interrupts-with-esp32/). 
Described implementation of hardware interrupt on pin state change using attachInterrupt() function. 
Not used in this solution as need to implement debouncing functionality. Have not found any way to implement this programically (maybe be need to think or google more about approaches).
Hardware debouncing could increased complexity and cost, as well not confidence that this will work as designed. One more blocker no way to easily tune debounce duration.

### ESP32 Timer interrupts
[ESP32 Arduino: Timer interrupts](https://techtutorialsx.com/2017/10/07/esp32-arduino-timer-interrupts/)
[Timer Interrupts with ESP32](https://iotespresso.com/timer-interrupts-with-esp32/)
[Timer Interrupts Explained with Examples](https://www.visualmicro.com/page/Timer-Interrupts-Explained.aspx)
[Espressif Doc: ESP32: Timer](https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/timer.html)
[ESP32TimerInterrupt Library](https://github.com/khoih-prog/ESP32TimerInterrupt)

Permamnent Store Configuration
[Install ESP32 Filesystem Uploader in Arduino IDE](https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/)
[Configuring & Handling ESP32 GPIO Interrupts In Arduino IDE](https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/)
[ESP32:Ïðèìåðû/SPIFFS ïëàòû ESP32 (ôàéëîâàÿ ñèñòåìà ïàìÿòè SPI Flash)](https://wikihandbk.com/wiki/ESP32:%D0%9F%D1%80%D0%B8%D0%BC%D0%B5%D1%80%D1%8B/SPIFFS_%D0%BF%D0%BB%D0%B0%D1%82%D1%8B_ESP32_(%D1%84%D0%B0%D0%B9%D0%BB%D0%BE%D0%B2%D0%B0%D1%8F_%D1%81%D0%B8%D1%81%D1%82%D0%B5%D0%BC%D0%B0_%D0%BF%D0%B0%D0%BC%D1%8F%D1%82%D0%B8_SPI_Flash))

FreeRTOS
Not investigated deeply as at first glance looked too complex for such device.

Multythreading using yield() function.

xPortGetCoreID()
TaskHandle_t
xTaskCreatePinnedToCore


Firebase Realtime Database Arduino Library for ESP32
https://github.com/mobizt/Firebase-ESP32
