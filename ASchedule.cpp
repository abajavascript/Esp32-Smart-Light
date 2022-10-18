#include "esp_wifi_types.h"
#include "ASchedule.h"
#include <Arduino.h>
#include <LinkedList.h>
#include "ANTPTime.h"
#include <TimeLib.h>
#include "ARelay.h"
#include "ADeviceConfig.h"

// ==================== ASchedule Class ====================
ASchedule::ASchedule() {
  //Initialize object
    t_prev = t_now = 0;
}

boolean ASchedule::isTimeElapsed(time_t t) {
  //expect that "time_t t" is in correct timezone
  if (year(t) < 2020) return false;
  if (!_active) return false;
  //all calculation will be performed in *_minutes values since midnight
  int t_minutes = hour(t) * 60 + minute(t);
  time_t sunrise_t = getSunriseTime(t);
  int sunrise_minutes = hour(sunrise_t) * 60 + minute(sunrise_t);
  time_t sunset_t = getSunsetTime(t);
  int sunset_minutes = hour(sunset_t) * 60 + minute(sunset_t);
  switch (_when) {
    case 0 : //exactTime
      //weekday() day of the week (1-7), Sunday is day 1; String dayOfWeek; //Example '0111110' (sun, mon, .., sat)
      if (_dayOfWeek.charAt(weekday(t) - 1) != '1') return false;
      return t_minutes >= _hour * 60 + _minute;
    case 1 : //beforeSunrise
      if (sunrise_minutes  - _hour * 60 - _minute >= 0) {
        if (_dayOfWeek.charAt(weekday(t) - 1) != '1') return false;
        return t_minutes >= sunrise_minutes  - _hour * 60 - _minute;
      } else {
        if (_dayOfWeek.charAt((weekday(t) - 1 + 1) % 7) != '1') return false;
        return t_minutes >= sunrise_minutes  - _hour * 60 - _minute + 24 * 60;
      }
    case 2 : //afterSunrise
      if (sunrise_minutes  + _hour * 60 + _minute < 24 * 60) {
        if (_dayOfWeek.charAt(weekday(t) - 1) != '1') return false;
        return t_minutes >= sunrise_minutes  + _hour * 60 + _minute;
      } else {
        if (_dayOfWeek.charAt((weekday(t) - 1 - 1 + 7) % 7) != '1') return false;
        return t_minutes >= sunrise_minutes  + _hour * 60 + _minute - 24 * 60;
      }
    case 3 : //beforeSunset
      if (sunset_minutes  - _hour * 60 - _minute >= 0) {
        if (_dayOfWeek.charAt(weekday(t) - 1) != '1') return false;
        return t_minutes >= sunset_minutes  - _hour * 60 - _minute;
      } else {
        if (_dayOfWeek.charAt((weekday(t) - 1 + 1) % 7) != '1') return false;
        return t_minutes >= sunset_minutes  - _hour * 60 - _minute + 24 * 60;
      }
    case 4 : //afterSunset
      if (sunset_minutes  + _hour * 60 + _minute < 24 * 60) {
        if (_dayOfWeek.charAt(weekday(t) - 1) != '1') return false;
        return t_minutes >= sunset_minutes  + _hour * 60 + _minute;
      } else {
        if (_dayOfWeek.charAt((weekday(t) - 1 - 1 + 7) % 7) != '1') return false;
        return t_minutes >= sunset_minutes  + _hour * 60 + _minute - 24 * 60;
      }
  }
  return false;
}

boolean ASchedule::process() {
  Serial.println("Processing schedule : " + _description);
  t_now = getNow();
  if (year(t_now) < 2020) return false;
  if (!(year(t_prev) < year(t_now) || month(t_prev) < month(t_now) || day(t_prev) < day(t_now) || isTimeElapsed(t_prev)) && isTimeElapsed(t_now)) {
    t_prev = t_now;
    return doAction();
    // return true;
  } else {
    t_prev = t_now;
    return false;
  }
}

boolean ASchedule::doAction(void){
  Serial.println("Do Acion " + _action + " on schedule chedule : " + _description);

  int relayStatus = relays.relayStatusAll();

  for(int i = 0; i < _action.length(); i++){
    // action : String; '??11??00' off = 0, on = 1, skip = ?, inver = !
    char a = _action.charAt(i);
    if (a == '0'){
      relays.relayOff(i);
    } else if (a == '1'){
      relays.relayOn(i);
    } else if (a == '!'){
      relays.relayInvert(i);
    }
  }

  return relayStatus != relays.relayStatusAll();
}

// ==================== AScheduleArr Class ====================
AScheduleArr::AScheduleArr(){
  //Initialize object
}

AScheduleArr::~AScheduleArr(){
	clear();
}

void AScheduleArr::add(ASchedule * schedule) {
  schedules.add(schedule);
  Serial.printf("Added Schedule #%i : %s\n", schedules.size() - 1, schedule->_description.c_str());
}

void AScheduleArr::del(byte index) {
  Serial.printf("Removing Schedule #%i of %i : %s\n", index, schedules.size(), schedules.get(index)->_description.c_str());
  delete schedules.get(index);
  schedules.remove(index);
}

void AScheduleArr::clear(void) {
  Serial.printf("Removing all %i Schedules\n", schedules.size());
  for (int i = 0; i < schedules.size(); i++) delete schedules.get(i);
  schedules.clear();
}

int AScheduleArr::getLength() {
  return schedules.size();
}

boolean AScheduleArr::process() {
  bool relayStatusChanged = false;
  for (int i = 0; i < schedules.size(); i++) {
    relayStatusChanged = relayStatusChanged || schedules.get(i)->process();
  }
  return relayStatusChanged;
}
