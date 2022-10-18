#include "ADeviceConfig.h"
#include <Preferences.h>
#include <json/FirebaseJson.h>
#include "AButton.h"
#include "ARelay.h"

AButtonArr buttons;
ARelayArr relays;
AScheduleArr schedules;

const char * deviceConfigDefault = 
      "{"
          //Pins where attached Relays. Should not exceed MAX_RELAY_CNT = 16
          "\"relays\" : [12, 13, 14, 15, 16, 17, 18, 19]," 
          //Configuration of Relay groups. Not more than MAX_RELAY_GROUPS_CNT 32 groups and not more MAX_RELAY_CNT = 16 relays in each group
          "\"relayGroups\" : [[0, 1], [2, 3], [4, 5], [6, 7], [0, 1, 2, 3, 4, 5, 6, 7]]," //array [0..31] of array [0..15]
          //Pins where attached Buttons. Should not exceed MAX_BUTTON_CNT = 16
          "\"buttons\" : [21, 22, 23, 25, 26, 27, 32, 33],"
          //Configure actions on Relay Groups depending on Click type (based on duration Short<1sec, Long=1..3 sec, Very Long>3sec)
          //Read this as "when on button #{param0} occured click with type {param1} on relay group #{param2} do action {param3}"
          //param0 = index of button, 0..buttons.length-1
          //param1 = click type: enum clickType {NO_CLICK=0, SHORT_CLICK=1, LONG_CLICK=2, LONGLONG_CLICK=3};
          //param2 = index of relay group, 0..relayGroups.length-1
          //param3 = action to perform for relay: enum relayGroupAction {RG_NO_ACTION=0, RG_ALL_ON=1, RG_ALL_OFF=2, RG_INVERT=3, RG_NEXT_COMBINATION=4, RG_ON_OR_INVERT=5, RG_OFF_OR_INVERT=6};
          //for any button not more than MAX_BUTTON_ACTIONS_CNT = 8 actions
          "\"buttonActions\" : ["
              "[0, 1, 0, 4], "//if button[0] == click=1 then relayGroup[0].action=Next=4
              "[0, 2, 0, 5], "
              "[0, 3, 4, 6], "
              "[1, 1, 0, 4], "
              "[1, 2, 0, 5], "
              "[1, 3, 4, 6], "
              "[2, 1, 1, 4], "
              "[2, 2, 1, 5], "
              "[2, 3, 4, 6], "
              "[3, 1, 1, 4], "
              "[3, 2, 1, 5], "
              "[3, 3, 4, 6], "
              "[4, 1, 2, 4], "
              "[4, 2, 2, 5], "
              "[4, 3, 4, 6], "
              "[5, 1, 2, 4], "
              "[5, 2, 2, 5], "
              "[5, 3, 4, 6], "
              "[6, 1, 3, 4], "
              "[6, 2, 3, 5], "
              "[6, 3, 4, 6], "
              "[7, 1, 3, 4], "
              "[7, 2, 3, 5], "
              "[7, 3, 4, 6] "
          "]"
      "}";

String deviceConfig;

const char * schedulesConfigDefault = 
//      "{"
      "["
//          "\"schedules\" : ["
              "{"
                   "\"description\" : \"Switch off everything at 24:00 everyday\"," //String with description what is the purpose of schedule
                   "\"active\" : false," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"1111111\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 0," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 0," //int : hour 0..23
                   "\"minute\" : 0," //int : 0..59
                   "\"action\" : \"00000000\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Switch on everything at 20:52 everyday\"," //String with description what is the purpose of schedule
                   "\"active\" : true," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"1111111\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 0," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 21," //int : hour 0..23
                   "\"minute\" : 42," //int : 0..59
                   "\"action\" : \"11111111\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Switch on everything at SUNrise everyday\"," //String with description what is the purpose of schedule
                   "\"active\" : true," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"0000100\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 1," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 9," //int : hour 0..23
                   "\"minute\" : 42," //int : 0..59
                   "\"action\" : \"10101010\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"On Sun, Mon, Tue at 1:15 before sunshine use multiple actions\"," 
                   "\"active\" : false,"
                   "\"dayOfWeek\" : \"1110000\"," //sun, mon, tue
                   "\"when\" : 3," 
                   "\"hour\" : 1," 
                   "\"minute\" : 15," 
                   "\"action\" : \"??11!!00\" " // off = 0, on = 1, skip = ?, inver = !
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
    deviceConfig = prefs.getString("device-config", deviceConfigDefault);
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
    schedulesConfig = prefs.getString("schedules-config", schedulesConfigDefault);
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
    ASchedule * schedule = new ASchedule();
    jsonData.getJSON(FirebaseJsonObject);
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

