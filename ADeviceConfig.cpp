#include "ADeviceConfig.h"
#include <Preferences.h>
#include <json/FirebaseJson.h>
#include "AButton.h"
#include "ARelay.h"

AButtonArr buttons;
ARelayArr relays;
AScheduleArr schedules;

//#include "ADeviceConfigDefault.h" 
#include "ADeviceConfigRoom1.h" 
String deviceConfig;

const char * schedulesConfigDefault = 
//      "{"
      "["
//          "\"schedules\" : ["
              "{"
                   "\"description\" : \"Switch off everything at 24:00 everyday\"," //String with description what is the purpose of schedule
                   "\"active\" : true," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"1111111\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 0," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 0," //int : hour 0..23
                   "\"minute\" : 0," //int : 0..59
                   "\"action\" : \"00000000\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Switch on everything at 11:11 everyday\"," //String with description what is the purpose of schedule
                   "\"active\" : true," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"1111111\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 0," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 11," //int : hour 0..23
                   "\"minute\" : 11," //int : 0..59
                   "\"action\" : \"11111111\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Do someaction at 01:01 Mon, Tue...Fri\"," //String with description what is the purpose of schedule
                   "\"active\" : true," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"0111110\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 0," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 1," //int : hour 0..23
                   "\"minute\" : 1," //int : 0..59
                   "\"action\" : \"11!!1??0\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Switch on/off 10:23 min before SUNrise on Mon, Web, Fri\"," //String with description what is the purpose of schedule
                   "\"active\" : true," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"0101010\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 1," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 10," //int : hour 0..23
                   "\"minute\" : 23," //int : 0..59
                   "\"action\" : \"10101010\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Switch on/off 1:34 min after SUNrise on Mon, Tue, Web\"," //String with description what is the purpose of schedule
                   "\"active\" : true," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"0111000\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 2," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 1," //int : hour 0..23
                   "\"minute\" : 34," //int : 0..59
                   "\"action\" : \"!!????11\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"On Sun, Mon, Tue at 23:15 before sunset use multiple actions\"," 
                   "\"active\" : true,"
                   "\"dayOfWeek\" : \"1110000\"," //sun, mon, tue
                   "\"when\" : 3," 
                   "\"hour\" : 23," 
                   "\"minute\" : 15," 
                   "\"action\" : \"??11!!00\" " // off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"On Sun, Tue, Wed at 9:23 after sunset use multiple actions\"," 
                   "\"active\" : true,"
                   "\"dayOfWeek\" : \"1011000\"," //sun, mon, tue
                   "\"when\" : 4," 
                   "\"hour\" : 9," 
                   "\"minute\" : 23," 
                   "\"action\" : \"??00!!00\" " // off = 0, on = 1, skip = ?, inver = !
              "}"
//          "]"
      "]";
//      "}";

String schedulesConfig;

boolean readDeviceConfig(String deviceConfigNew) {
  if (deviceConfigNew == "") {
    //Read device configuration from EEPROM or use default in deviceConfigDefault
    Preferences prefs;
    prefs.begin("smart-light");
    deviceConfig = prefs.getString("device-conf", deviceConfigDefault);
    prefs.end();
  } else {
    deviceConfig = deviceConfigNew;
  }

  //JSON processing variables
  FirebaseJson deviceConfigJson;
  FirebaseJsonData jsonData;
  FirebaseJsonArray jsonArray, jsonArrayActions, jsonArrayRelayGroups;

  //Deserialize Device configuration from String variable to JSON : deviceConfigJson
  Serial.println(deviceConfig);
  if (deviceConfigJson.setJsonData(deviceConfig)) {
    Serial.println("Deserealization succeed.");
  } else {
    Serial.println("Deserealization failed.");
    return false;
  } 

  if (deviceConfigNew != "") { //deviceConfigNew is not empty in case we executed this function with data received from Firebase
    //Write device configuration received from Firebase to EEPROM 
    Preferences prefs;
    prefs.begin("smart-light");
    prefs.putString("device-conf", deviceConfig);
    prefs.end();
    Serial.printf("Stored device config in EEPROM. New Value : %s\n", deviceConfig.c_str());
  }

  //Print deserialized JSON with pretifing
  String str;
  deviceConfigJson.toString(str, true /* prettify option */);
  Serial.println(str);

  //Clear all configs
  buttons.clear();
  relays.clearRelayGroups();
  relays.clear();
  
  //Read button configuration and initialize buttons
  Serial.printf("Initializing buttons...\n");
  deviceConfigJson.get(jsonData, "buttons");
  jsonData.get<FirebaseJsonArray>(jsonArray);
  for (size_t i = 0; i < jsonArray.size(); i++){
    jsonArray.get(jsonData, i);
    buttons.add(jsonData.to<int>());
  }
  Serial.printf("Done. #Buttons = %d\n", buttons.getLength());
 
  //Read relay configuration and initialize relays
  Serial.printf("Initializing relays...\n");
  deviceConfigJson.get(jsonData, "relays");
  jsonData.get<FirebaseJsonArray>(jsonArray);
  for (size_t i = 0; i < jsonArray.size(); i++){
    jsonArray.get(jsonData, i);
    relays.add(jsonData.to<int>());
  }
  Serial.printf("Done. #Relays = %d\n", relays.getLength());

  //Read relay groups configuration 
  Serial.printf("Reading Relay Groups configuration...\n");
  deviceConfigJson.get(jsonData, "relayGroups");
  jsonData.get<FirebaseJsonArray>(jsonArray);
  for (size_t i = 0; i < jsonArray.size(); i++){
    jsonArray.get(jsonData, i);
    jsonData.get<FirebaseJsonArray>(jsonArrayRelayGroups);
    uint16_t rel_grp = 0;//Relay Group configuration is stored as bit representation: 1-included into group, 0-no
    for (size_t j = 0; j < jsonArrayRelayGroups.size(); j++){
      jsonArrayRelayGroups.get(jsonData, j);
      int rel_idx = jsonData.to<int>();
      rel_grp |= 1 << rel_idx;
    }
    relays.addRelayGroup(rel_grp);//rel_grp = 0000 0000 1100 0011 - included relays 0 1 6 7 into relay group
  }
  Serial.printf("Done. #RelayGroupss = %d\n", relays.getRelayGroupsLength());

  //Read button actions configuration 
  Serial.printf("Reading Button Actions configuration...\n");
  deviceConfigJson.get(jsonData, "buttonActions");
  jsonData.get<FirebaseJsonArray>(jsonArray);
  for (size_t i = 0; i < jsonArray.size(); i++){
    jsonArray.get(jsonData, i);
    jsonData.get<FirebaseJsonArray>(jsonArrayActions);
    if (jsonArrayActions.size() == 4){
      jsonArrayActions.get(jsonData, 0);
      int button_index = jsonData.to<int>();
      jsonArrayActions.get(jsonData, 1);
      AButtonClickType button_click_type = (AButtonClickType)jsonData.to<int>();
      jsonArrayActions.get(jsonData, 2);
      int relay_group_index = jsonData.to<int>();
      jsonArrayActions.get(jsonData, 3);
      ARelayGroupAction relay_group_action = (ARelayGroupAction)jsonData.to<int>();
      buttons.addButtonAction(button_index, button_click_type, relay_group_index, relay_group_action);
    }
  }
  Serial.printf("Done. #ButtonActions = %d\n", buttons.getButtonActionsLength());

  return true;
}

boolean readSchedulesConfig(String schedulesConfigNew) {
  if (schedulesConfigNew == "") {
    //Read device configuration from EEPROM or use default in schedulesConfigDefault - which by default is empty array
    Preferences prefs;
    prefs.begin("smart-light");
    schedulesConfig = prefs.getString("schedules-conf", schedulesConfigDefault);
    prefs.end();
  } else {
    schedulesConfig = schedulesConfigNew;
  }
             
  //JSON processing variables
  FirebaseJson FirebaseJsonObject;
  FirebaseJsonData jsonData, jsonDataVal;
  FirebaseJsonArray jsonArray;

  //Deserialize schedules configuration from String variable to JSON : schedulesConfigJson
  Serial.println(schedulesConfig);
  if (jsonArray.setJsonArrayData(schedulesConfig)) {
    Serial.println("Schedules Deserealization succeed.");
  } else {
    Serial.println("Schedules Deserealization failed.");
    return false;
  } 

  if (schedulesConfigNew != "") { //deviceConfigNew is not empty in case we executed this function with data received from Firebase
    //Write device configuration received from Firebase to EEPROM 
    Preferences prefs;
    prefs.begin("smart-light");
    prefs.putString("schedules-conf", schedulesConfig);
    prefs.end();
    Serial.printf("Stored schedules config in EEPROM. New Value : %s\n", schedulesConfig.c_str());
  }

  //Print deserialized JSON with pretifing
  String str;
  jsonArray.toString(str, true /* prettify option */);
  Serial.println(str);

  //Clear all schedules
  schedules.clear();
                                 
  //Read schedules configuration 
  Serial.printf("Initializing schedules...\n");
  for (size_t i = 0; i < jsonArray.size(); i++){
    jsonArray.get(jsonData, i);
    jsonData.getJSON(FirebaseJsonObject);
    //skip if there is no description
    FirebaseJsonObject.get(jsonDataVal, "description");
    if (jsonDataVal.to<String>() == "") continue;

    ASchedule * schedule = new ASchedule();
    FirebaseJsonObject.get(jsonDataVal, "description");
    schedule->_description = jsonDataVal.to<String>();
    FirebaseJsonObject.get(jsonDataVal, "active");
    schedule->_active = jsonDataVal.to<boolean>();
    FirebaseJsonObject.get(jsonDataVal, "dayOfWeek");
    schedule->_dayOfWeek = jsonDataVal.to<String>();
    FirebaseJsonObject.get(jsonDataVal, "when");
    schedule->_when = jsonDataVal.to<int>();
    FirebaseJsonObject.get(jsonDataVal, "hour");
    schedule->_hour = jsonDataVal.to<int>();
    FirebaseJsonObject.get(jsonDataVal, "minute");
    schedule->_minute = jsonDataVal.to<int>();
    FirebaseJsonObject.get(jsonDataVal, "action");
    schedule->_action = jsonDataVal.to<String>();
   
    schedules.add(schedule);
  }
  Serial.printf("Done. #Schedules = %d\n", schedules.getLength());

  return true;
}

void clearAllPreferences(void) {
  Preferences prefs;
  prefs.begin("smart-light");
  prefs.remove("device-conf");
  prefs.remove("schedules-conf");
  prefs.end();
  Serial.printf("Stored schedules config in EEPROM. New Value : %s\n", schedulesConfig.c_str());
}
