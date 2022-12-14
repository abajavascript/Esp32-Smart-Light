#ifndef ABasicOTA_h
#define ABasicOTA_h

//General WiFi staff
//#include <WiFi.h>
//#include <ESPmDNS.h>
//#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//personal passwords predefined
#include "secret.h"
//or override below
#ifndef SECRET_H
#define OTA_PASSWORD "Admin"
#endif

void initBasicOTA() {

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  ArduinoOTA.setPassword(OTA_PASSWORD);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start OTA updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nOTA End");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%% : %d from %d bytes\n", (progress / (total / 100)), progress, total);
    })
    .onError([](ota_error_t error) {
      Serial.printf("OTA Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("OTA Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("OTA Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("OTA Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("OTA End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Basic OTA Ready");
}

void handleBasicOTA() {
  ArduinoOTA.handle();
}

#endif
