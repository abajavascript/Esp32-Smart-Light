#ifndef AWiFi_h
#define AWiFi_h

//General WiFi staff
#include <WiFi.h>
#include <WiFiManager.h>

//personal passwords predefined
#include "secret.h"
//or override below
#ifndef SECRET_H
#define WM_PASSWORD "123456"
#endif

String mac, deviceId, ip;

void postConnectedWiFi();
void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info);

void initWiFi() {
  //WiFi initialization
  Serial.println("Initializing Wi-Fi");

  //Register event handlers
  WiFi.onEvent(onWifiConnect, ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.mode(WIFI_STA);
  WiFiManager wm;
  wm.setConfigPortalTimeout(180);

  bool res = wm.autoConnect(wm.getDefaultAPName().c_str(), WM_PASSWORD); // password protected ap
  if (!res) {
    Serial.println("Failed to connect");
    //ESP.restart();
  } else {
    //postConnectedWiFi();
  }
  delay(2000);
}

void checkWiFi() {
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    //postConnectedWiFi();
  }
}

void postConnectedWiFi() {
  //  delay(1000);
  if ((WiFi.status() == WL_CONNECTED)) {
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    mac = WiFi.macAddress();
    Serial.printf("Device MAC = %s\n", mac.c_str());
    deviceId = WiFi.macAddress();//use ESP unique MAC as device id
    deviceId.replace(":", "-");
    //ip = WiFi.localIP().toString();
    ip = "";//Make IP empty to force IP storing into Firebase
  }
}

void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to Wi-Fi sucessfully.");
  postConnectedWiFi();
}

#endif
