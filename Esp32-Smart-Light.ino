#include "ADeviceConfig.h"
#include "AWiFi.h"
#include "ANTPTime.h"
#include "AFirebase.h"
#include "ATimerInterrupt.h"
#include "AButton.h"
#include "ARelay.h"

//General configuration        
#define UPDATE_INTERVAL 600// 60 //interval in seconds to update sensors
uint32_t updateTiming;

 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  readDeviceConfig();

  initWiFi();  

  initFirebase();

  //to force immediate execution (after 3 second)
  updateTiming = millis() - UPDATE_INTERVAL * 1000 + 3 * 1000;

  initTimerInterrupt();
}

void loop() {
  if (fbEventReceived) {
    fbEventReceived = false;
    Serial.printf("Clear fbEventReceived flag\n");
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
