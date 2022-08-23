#ifndef AFirebase_h
#define AFirebase_h

#include "ANTPTime.h"

//Firebase related
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

//personal passwords predefined
#include "secret.h"
//or override below
#ifndef SECRET_H
#define API_KEY "fire base api key"
#define USER_EMAIL "user@server.com"
#define USER_PASSWORD "123456"
#define DATABASE_URL "https://database name.firebaseio.com" 
#endif

/********* Firebase **************/
/* Define the Firebase Data object */
FirebaseData fbdo;
FirebaseData stream;
/* Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;
/* Define the FirebaseConfig data for config data */
FirebaseConfig config;
volatile bool fbCommandRefreshReceived = false;
volatile bool fbCommandSaveReceived = false;
volatile bool fbCommandLoadReceived = false;
volatile bool fbCommandSetReceived = false;
volatile uint32_t fbCommandSetNewState = 0;

String startTime = "";

bool fbStoreSensorData(void){
  //upload to DB

  if (!Firebase.ready()) return false;

  String currentPath;

  String timePath =  getTimeAsPath(); // yyyy/mm/dd/hh/nn or empty is error
  if (!timePath.length()) return false;
  Serial.printf("Processing Device = %s\n", deviceId.c_str());

  if (startTime == ""){
    currentPath = "/sensors/" + deviceId + "/start/time";
    Serial.printf("Strore start time to %s... %s\n", currentPath.c_str(), Firebase.setString/*Async*/(fbdo, currentPath.c_str(), timePath.c_str()) ? "ok" : fbdo.errorReason().c_str());
    currentPath = "/sensorsList/" + deviceId + "/mac";
    Serial.printf("Strore sensor id to %s... %s\n", currentPath.c_str(), Firebase.setString/*Async*/(fbdo, currentPath.c_str(), mac.c_str()) ? "ok" : fbdo.errorReason().c_str());
    currentPath = "/uptime/" + deviceId + "/" + timePath + "/last_time";
    Serial.printf("Insert uptime informatione to %s... %s\n", currentPath.c_str(), Firebase.setString/*Async*/(fbdo, currentPath.c_str(), timePath.c_str()) ? "ok" : fbdo.errorReason().c_str());
    startTime = timePath;//update start time when new sensors added
  } else {
    currentPath = "/uptime/" + deviceId + "/" + startTime + "/last_time";
    Serial.printf("Update uptime informatione to %s... %s\n", currentPath.c_str(), Firebase.setString/*Async*/(fbdo, currentPath.c_str(), timePath.c_str()) ? "ok" : fbdo.errorReason().c_str());
  }

  FirebaseJson jsonR;
  jsonR.set("r", relays.relayStatusAllStr());
  currentPath = "/relays/" + deviceId + "/" + timePath;
  Serial.printf("Strore relay state to %s... %s\n", currentPath.c_str(), Firebase.set/*Async*/(fbdo, currentPath.c_str(), jsonR) ? "ok" : fbdo.errorReason().c_str());

  FirebaseJson jsonRL;
  jsonRL.set("r", relays.relayStatusAllStr());
  jsonRL.set("time", timePath);
  currentPath = "/sensors/" + deviceId + "/last";
  Serial.printf("Strore last relay status and time to %s... %s\n", currentPath.c_str(), Firebase.set/*Async*/(fbdo, currentPath.c_str(), jsonRL) ? "ok" : fbdo.errorReason().c_str());

  return true;
}

void fbClearCommand(void) {
  fbCommandRefreshReceived = false;
  fbCommandSaveReceived = false;
  fbCommandLoadReceived = false;
  fbCommandSetReceived = false;
  
  if (Firebase.ready()) {
    String timePath =  getTimeAsPath(); // yyyy/mm/dd/hh/nn or empty is error
    Serial.printf("Clear reresh command ... %s\n", Firebase.setString(fbdo, String("/commands/" + mac).c_str(), String("Done " + timePath).c_str()) ? "ok" : fbdo.errorReason().c_str());
  }
  
}

void firebaseStreamCallback(StreamData data) {
  Serial.println("Function : firebaseStreamCallback:");
  Serial.printf("sream path : %s\nevent path : %s\ndata type : %s\nevent type : %s\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  Serial.print("data : ");
  printResult(data); //see addons/RTDBHelper.h

  //This is the size of stream payload received (current and max value)
  //Max payload size is the payload size under the stream path since the stream connected
  //and read once and will not update until stream reconnection takes place.
  //This max value will be zero as no payload received in case of ESP8266 which
  //BearSSL reserved Rx buffer size is less than the actual stream payload.
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", data.payloadLength(), data.maxPayloadLength());

  //Due to limited of stack memory, do not perform any task that used large memory here especially starting connect to server.
  //Just set this flag and check it status later.
  
  if (data.dataTypeEnum() != fb_esp_rtdb_data_type_string) return;
  String str = data.to<String>();

  if (str == "refresh") //command that request update firebase realtime database with fresh information about relay state
    fbCommandRefreshReceived = true;
  
  if (str == "save") //command to save Device Config JSON to Firebase
    fbCommandSaveReceived = true;
  
  if (str == "load") //command to load Device Config from Firebase
    fbCommandLoadReceived = true;
  
  if (str.startsWith("set:")) { //set On/Off relays 1-On, 0-Off, Other-Off
    fbCommandSetReceived = true;
    fbCommandSetNewState = 0;
    for (int i = 4; i < str.length(); i++) {
      fbCommandSetNewState = fbCommandSetNewState << 1;
      if (str[i] == '1') fbCommandSetNewState |= 1;
    }
  }
}

void firebaseStreamTimeoutCallback(bool timeout) {
  //Serial.printf("Function : firebaseStreamTimeoutCallback: %u\n", millis());
  if (timeout)
    Serial.println("Firebase stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("Firebase error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void initFirebase(void){
  //Firebase initialization
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);
  #if defined(ESP8266)
    // required for large file data, increase Rx size as needed.
    fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 512 /* Tx buffer size in bytes from 512 - 16384 */);
  #endif
  // Assign the callback function for the long running token generation task 
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  // Assign the maximum retry of token generation 
  config.max_token_generation_retry = 5;
  // Initialize the library with the Firebase authen and config 
  Firebase.begin(&config, &auth);
  //listener on data change
  Serial.printf("Creating listener for %s\n", mac.c_str());
  if (!Firebase.RTDB.beginStream(&stream, "/commands/" + mac))
    Serial.printf("Firebase listen sream begin error, %s\n\n", stream.errorReason().c_str());
  Firebase.RTDB.setStreamCallback(&stream, firebaseStreamCallback, firebaseStreamTimeoutCallback);
}

void saveConfigToFirebase(void) {
  String currentPath = "/sensors/" + deviceId + "/config";

  FirebaseJson deviceConfigJson;
  deviceConfigJson.setJsonData(deviceConfig);
  Serial.printf("Strore config to %s... %s\n", currentPath.c_str(), Firebase.set/*Async*/(fbdo, currentPath.c_str(), deviceConfigJson) ? "ok" : fbdo.errorReason().c_str());

  fbClearCommand();
}

void loadConfigFromFirebase(void) {
  String currentPath = "/sensors/" + deviceId + "/config";

  Serial.printf("Load config from %s... %s\n", currentPath.c_str(), Firebase.getJSON(fbdo, currentPath.c_str()) ? "ok" : fbdo.errorReason().c_str());
  readDeviceConfig(fbdo.to<String>());

  fbClearCommand();
}

#endif
