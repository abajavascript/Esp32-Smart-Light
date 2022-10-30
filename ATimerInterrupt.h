#ifndef ATimerInterrupt_h
#define ATimerInterrupt_h

#include "ADeviceConfig.h"
#include "AButton.h"
#include "ARelay.h"

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile bool relayStatusChanged = false;

void IRAM_ATTR onTimer() {
  int relayStatus = relays.relayStatusAll();
  for (int idx = 0; idx < buttons.getLength(); idx++) {
    AButtonClickType click = buttons.isClicked(idx);
    AButtonAction * btnAction;
    switch (click) {
      case SHORT_CLICK:
      case LONG_CLICK:
      case LONGLONG_CLICK:
        for (int i = 0; i < buttons.getButtonActionsLength(idx); i++) {
          btnAction = buttons.getAction(idx, i);
          if (btnAction->click == click) {
            switch (btnAction->relayGroupAction) {
              case RG_ALL_ON: 
                relays.relayGroupOn(btnAction->relayGroupIndex);
                break;
              case RG_ALL_OFF: 
                relays.relayGroupOff(btnAction->relayGroupIndex);
                break;
              case RG_INVERT: 
                relays.relayGroupInvert(btnAction->relayGroupIndex);
                break;
              case RG_ON_OR_INVERT: 
                relays.relayGroupOnOrInvert(btnAction->relayGroupIndex);
                break;
              case RG_OFF_OR_INVERT:
                relays.relayGroupOffOrInvert(btnAction->relayGroupIndex);
                break;
              case RG_NEXT_BIN_COMBINATION: 
                relays.relayGroupNextBinCombination(btnAction->relayGroupIndex);
                break;
              case RG_NEXT_SET_COMBINATION: 
                relays.relayGroupNextSetCombination(btnAction->relayGroupIndex);
                break;
            }
          }
        }
        break;
    }
  }
    if (relayStatus != relays.relayStatusAll()) {
      portENTER_CRITICAL_ISR(&timerMux);
      relayStatusChanged = true;
      portEXIT_CRITICAL_ISR(&timerMux);
    }
}

void initTimerInterrupt() {
  //timer interrupt
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10000, true);
  timerAlarmEnable(timer);
}


#endif
