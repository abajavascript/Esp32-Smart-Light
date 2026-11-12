#include "ADeviceConfig.h"
#include "AParseJSON.h"
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
                   "\"active\" : false," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"1111111\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 0," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 11," //int : hour 0..23
                   "\"minute\" : 11," //int : 0..59
                   "\"action\" : \"11111111\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Do someaction at 01:01 Mon, Tue...Fri\"," //String with description what is the purpose of schedule
                   "\"active\" : false," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"0111110\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 0," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 1," //int : hour 0..23
                   "\"minute\" : 1," //int : 0..59
                   "\"action\" : \"11!!1??0\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Switch on/off 10:23 min before SUNrise on Mon, Web, Fri\"," //String with description what is the purpose of schedule
                   "\"active\" : false," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"0101010\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 1," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 10," //int : hour 0..23
                   "\"minute\" : 23," //int : 0..59
                   "\"action\" : \"10101010\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"Switch on/off 1:34 min after SUNrise on Mon, Tue, Web\"," //String with description what is the purpose of schedule
                   "\"active\" : false," //boolean : off = false, on = true
                   "\"dayOfWeek\" : \"0111000\"," //String; Example '0111110' (sun, mon, .., sat)
                   "\"when\" : 2," //int; exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
                   "\"hour\" : 1," //int : hour 0..23
                   "\"minute\" : 34," //int : 0..59
                   "\"action\" : \"!!????11\" " //String; '??11??00' off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"On Sun, Mon, Tue at 23:15 before sunset use multiple actions\"," 
                   "\"active\" : false,"
                   "\"dayOfWeek\" : \"1110000\"," //sun, mon, tue
                   "\"when\" : 3," 
                   "\"hour\" : 23," 
                   "\"minute\" : 15," 
                   "\"action\" : \"??11!!00\" " // off = 0, on = 1, skip = ?, inver = !
              "},"
              "{"
                   "\"description\" : \"On Sun, Tue, Wed at 9:23 after sunset use multiple actions\"," 
                   "\"active\" : false,"
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
  String previousConfig = deviceConfig;
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
    if (deviceConfigNew != "") deviceConfig = previousConfig;
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

static void setScheduleError(String *errorOut, const char *reason) {
  Serial.printf("Schedules rejected: %s\n", reason);
  if (errorOut) *errorOut = reason;
}

static void discardParsedSchedules(LinkedList<ASchedule *> &parsed) {
  for (int i = 0; i < parsed.size(); i++) delete parsed.get(i);
  parsed.clear();
}

// 1 = stored, 0 = web dummy with an empty description, -1 = not a schedule.
static int appendSchedule(FirebaseJson &obj, LinkedList<ASchedule *> &parsed) {
  FirebaseJsonData data;
  if (!jsonGet(obj, "description", data)) return -1;
  String description = data.stringValue;
  description.trim();
  if (!description.length()) return 0;

  bool active = false;
  int when = 0;
  int hour = 0;
  int minute = 0;
  String dayOfWeek;
  String action;
  if (!readJsonBool(obj, "active", active)) return -1;
  if (!readJsonString(obj, "dayOfWeek", dayOfWeek)) return -1;
  if (!readJsonInt(obj, "when", when)) return -1;
  if (!readJsonInt(obj, "hour", hour)) return -1;
  if (!readJsonInt(obj, "minute", minute)) return -1;
  if (!readJsonString(obj, "action", action)) return -1;

  ASchedule *schedule = new ASchedule();
  schedule->_description = description;
  schedule->_active = active;
  schedule->_dayOfWeek = dayOfWeek;
  schedule->_dayOfWeek.trim();
  schedule->_when = when;
  schedule->_hour = hour;
  schedule->_minute = minute;
  schedule->_action = action;
  schedule->_action.trim();
  parsed.add(schedule);
  return 1;
}

static bool parseScheduleArray(FirebaseJsonArray &jsonArray, LinkedList<ASchedule *> &parsed, String *errorOut) {
  for (size_t i = 0; i < jsonArray.size(); i++) {
    FirebaseJsonData data;
    jsonArray.get(data, i);
    FirebaseJson obj;
    if (!data.getJSON(obj)) {
      setScheduleError(errorOut, "bad schedule");
      return false;
    }
    if (appendSchedule(obj, parsed) < 0) {
      setScheduleError(errorOut, "bad schedule");
      return false;
    }
  }
  return true;
}

// Realtime Database stores a JSON array as {"0":{...},"1":{...}} when indexes have holes.
static bool loadScheduleArray(const String &text, FirebaseJsonArray &jsonArray, String &normalized, String *errorOut) {
  if (text.charAt(0) == '[') {
    normalized = text;
    if (!jsonArray.setJsonArrayData(text)) {
      setScheduleError(errorOut, "bad json");
      return false;
    }
    return true;
  }
  if (text.charAt(0) != '{') {
    setScheduleError(errorOut, "bad json");
    return false;
  }

  FirebaseJson obj;
  if (!obj.setJsonData(text)) {
    setScheduleError(errorOut, "bad json");
    return false;
  }

  const int maxSchedules = 64;
  String overflowKey = String(maxSchedules);
  FirebaseJsonData overflow;
  if (jsonGet(obj, overflowKey.c_str(), overflow)) {
    setScheduleError(errorOut, "too many schedules");
    return false;
  }

  String arrayBody = "[";
  bool any = false;
  for (int idx = 0; idx < maxSchedules; idx++) {
    String key = String(idx);
    FirebaseJsonData data;
    if (!jsonGet(obj, key.c_str(), data)) continue;
    FirebaseJson child;
    if (!data.getJSON(child)) {
      setScheduleError(errorOut, "bad schedule");
      return false;
    }
    String childStr;
    child.toString(childStr, false);
    if (!childStr.length()) {
      setScheduleError(errorOut, "bad schedule");
      return false;
    }
    if (any) arrayBody += ',';
    any = true;
    arrayBody += childStr;
  }
  arrayBody += ']';
  if (!any) {
    setScheduleError(errorOut, "not a schedule list");
    return false;
  }

  normalized = arrayBody;
  if (!jsonArray.setJsonArrayData(normalized)) {
    setScheduleError(errorOut, "bad json");
    return false;
  }
  return true;
}

boolean readSchedulesConfig(String schedulesConfigNew, String *errorOut) {
  bool fromOutside = schedulesConfigNew.length() > 0;
  String text;
  if (!fromOutside) {
    //Read device configuration from EEPROM or use default in schedulesConfigDefault - which by default is empty array
    Preferences prefs;
    prefs.begin("smart-light");
    schedulesConfig = prefs.getString("schedules-conf", schedulesConfigDefault);
    prefs.end();
    text = schedulesConfig;
  } else {
    text = schedulesConfigNew;
  }
  text.trim();
  Serial.println(text);
  if (!text.length()) {
    setScheduleError(errorOut, fromOutside ? "empty payload" : "bad json");
    return false;
  }

  FirebaseJsonArray jsonArray;
  String normalized;
  if (!loadScheduleArray(text, jsonArray, normalized, errorOut)) return false;
  Serial.println("Schedules Deserealization succeed.");

  LinkedList<ASchedule *> parsed;
  if (!parseScheduleArray(jsonArray, parsed, errorOut)) {
    discardParsedSchedules(parsed);
    return false;
  }

  if (fromOutside) {
    Preferences prefs;
    prefs.begin("smart-light");
    size_t wrote = prefs.putString("schedules-conf", normalized);
    prefs.end();
    if (wrote == 0) {
      discardParsedSchedules(parsed);
      setScheduleError(errorOut, "nvs write failed");
      return false;
    }
    Serial.printf("Stored schedules config in EEPROM. New Value : %s\n", normalized.c_str());
  }

  String pretty;
  jsonArray.toString(pretty, true);
  Serial.println(pretty);

  // Replace the live list only after the payload parsed and, for a download, NVS accepted it.
  schedulesConfig = normalized;
  Serial.printf("Initializing schedules...\n");
  schedules.clear();
  for (int i = 0; i < parsed.size(); i++) schedules.add(parsed.get(i));
  parsed.clear();
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
