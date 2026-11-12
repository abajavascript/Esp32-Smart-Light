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

extern String mac, deviceId, ip;

void initWiFi();
void handleWiFi();
void checkWiFi();
void postConnectedWiFi();
void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info);

#endif
