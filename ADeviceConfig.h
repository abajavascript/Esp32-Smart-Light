#ifndef ADeviceConfig_h
#define ADeviceConfig_h

#include "AButton.h"
#include "ARelay.h"
#include "ASchedule.h"
boolean readDeviceConfig(String deviceConfigNew = "");
// Empty schedulesConfigNew reads NVS or the factory list. A payload that does not
// parse is rejected: errorOut receives a short reason and the live list stays.
boolean readSchedulesConfig(String schedulesConfigNew = "", String *errorOut = nullptr);
void clearAllPreferences(void);

extern String deviceConfig;
extern String schedulesConfig;

extern AButtonArr buttons;
extern ARelayArr relays;
extern AScheduleArr schedules;

#endif
