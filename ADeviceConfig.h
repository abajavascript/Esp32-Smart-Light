#ifndef ADeviceConfig_h
#define ADeviceConfig_h

#include "AButton.h"
#include "ARelay.h"
boolean readDeviceConfig(String deviceConfigNew = "");

extern String deviceConfig;

extern AButtonArr buttons;
extern ARelayArr relays;

#endif
