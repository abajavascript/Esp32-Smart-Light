#ifndef ADeviceConfig_h
#define ADeviceConfig_h

#include "AButton.h"
#include "ARelay.h"
#include "ASchedule.h"
boolean readDeviceConfig(String deviceConfigNew = "");
boolean readSchedulesConfig(String schedulesConfigNew = "");
void clearAllPreferences(void);

extern String deviceConfig;
extern String schedulesConfig;

extern AButtonArr buttons;
extern ARelayArr relays;
extern AScheduleArr schedules;

#endif
