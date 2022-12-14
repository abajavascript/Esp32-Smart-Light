#include "ADeviceConfig.h"
#include "AWiFi.h"
#include "ABasicOTA.h"
#include "ANTPTime.h"
#include "AFirebase.h"
#include "ATimerInterrupt.h"
#include "AButton.h"
#include "ARelay.h"
#include "ASchedule.h"

//General configuration        
//aaaaaaaaaa #define UPDATE_INTERVAL 600// 60 //interval in seconds to update sensors
#define UPDATE_INTERVAL (60*60*24*30)// Тестово раз вмісяць
#define UPDATE_SCHEDULES_INTERVAL 60 //interval in seconds to process schedules
#define RECONNECT_WIFI_INTERVAL 900// check WiFi and reconnect every 15 min
uint32_t updateTiming, updateSchedulesTiming;
uint32_t reconnectWiFiMillis;

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  readDeviceConfig();

  readSchedulesConfig();

  initWiFi();  
  //to force WiFi reconnect in loop, because router has 2-3 min to startup (after 3 minutes)
  reconnectWiFiMillis = millis() - RECONNECT_WIFI_INTERVAL * 1000 + 3 * 60 * 1000;

  initBasicOTA();

  initFirebase();

  //to force immediate execution (after 3 second)
  updateTiming = millis() - UPDATE_INTERVAL * 1000 + 3 * 1000;
  updateSchedulesTiming = millis() - UPDATE_SCHEDULES_INTERVAL * 1000 + 3 * 1000;

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

  //process al schedules
  if (millis() - updateSchedulesTiming > UPDATE_SCHEDULES_INTERVAL * 1000) {
    Serial.println("Start process schedules ! ");
    updateSchedulesTiming = millis();
    if (schedules.process()){
      Serial.println("Schedules changed relay status ! ");
      portENTER_CRITICAL_ISR(&timerMux);
      relayStatusChanged = true;
      portEXIT_CRITICAL_ISR(&timerMux);
    }
  }

  //Process commands received from webapp
  if (fbCommandResetReceived) {
    resetConfigAndSchedules();
  }
  if (fbCommandSaveConfigReceived && fbCommandSaveSchedulesReceived) {
    saveConfigToFirebase();
    saveSchedulesToFirebase();
  }
  if (fbCommandLoadConfigReceived && fbCommandLoadSchedulesReceived) {
    loadConfigFromFirebase();
    loadSchedulesFromFirebase();
  }
  if (fbCommandSaveConfigReceived) {
    saveConfigToFirebase();
  }
  if (fbCommandLoadConfigReceived) {
    loadConfigFromFirebase();
  }
  if (fbCommandSaveSchedulesReceived) {
    saveSchedulesToFirebase();
  }
  if (fbCommandLoadSchedulesReceived) {
    loadSchedulesFromFirebase();
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
