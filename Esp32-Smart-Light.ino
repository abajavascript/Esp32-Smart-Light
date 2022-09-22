#include "ADeviceConfig.h"
#include "AWiFi.h"
#include "ABasicOTA.h"
#include "ANTPTime.h"
#include "AFirebase.h"
#include "ATimerInterrupt.h"
#include "AButton.h"
#include "ARelay.h"

//General configuration        
#define UPDATE_INTERVAL 600// 60 //interval in seconds to update sensors
#define RECONNECT_WIFI_INTERVAL 900// check WiFi and reconnect every 15 min
uint32_t updateTiming;
uint32_t reconnectWiFiMillis;

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  readDeviceConfig();

  initWiFi();  
  //to force WiFi reconnect in loop, because router has 2-3 min to startup (after 3 minutes)
  reconnectWiFiMillis = millis() - RECONNECT_WIFI_INTERVAL * 1000 + 3 * 60 * 1000;

  initBasicOTA();

  initFirebase();

  //to force immediate execution (after 3 second)
  updateTiming = millis() - UPDATE_INTERVAL * 1000 + 3 * 1000;

  initTimerInterrupt();
}

void loop() {
  //Handle OTA requests if needed
  handleBasicOTA();
  
  //check WiFi and reconnect if needed - looks that it reconnects without this code with checkWiFi
  if (millis() - reconnectWiFiMillis > RECONNECT_WIFI_INTERVAL * 1000){
    checkWiFi();
    reconnectWiFiMillis = millis();
  }

  if (fbCommandSaveReceived) {
    saveConfigToFirebase();
  }
  if (fbCommandLoadReceived) {
    loadConfigFromFirebase();
  }
  //wait for some period of second to collect and upload sensors only onece per UPDATE_INTERVAL seconds
  if ((millis() - updateTiming < UPDATE_INTERVAL * 1000) && (!fbCommandRefreshReceived) && (!fbCommandSetReceived) && (!relayStatusChanged)) return;
  updateTiming = millis();

  if (relayStatusChanged) {
    Serial.println("");
    Serial.println("Button pressed and Relay state changed ! ");
    Serial.println(relays.relayStatusAllStr());
    portENTER_CRITICAL(&timerMux);
    relayStatusChanged = false;
    portEXIT_CRITICAL(&timerMux);
  }  

  if (fbCommandSetReceived) relays.relaySetStatusAll(fbCommandSetNewState);
  
  fbStoreSensorData();
  Serial.printf("Function fbStoreSensorData execution time: %u\n", millis() - updateTiming);

  if (fbCommandRefreshReceived || fbCommandSetReceived) fbClearCommand();
  
}
