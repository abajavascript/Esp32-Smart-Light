#ifndef ARelay_h
#define ARelay_h
#include <Arduino.h>
#include "ADeviceHWConfig.h"

#define RELAY_ON LOW
#define RELAY_OFF HIGH

class ARelay {
  public:
	ARelay(uint8_t pin = 0xFF);	
	
	uint8_t getPin();
    void setPin(uint8_t pin); 

	boolean isOn();
	boolean isOff();
	boolean invert();
	boolean on();
	boolean off();
	
  private:
	uint8_t _PIN = 0;
	boolean _state;

	void setState(boolean state);
};

class ARelayArr {
private: 
  ARelay * _relays[MAX_RELAY_CNT];
  char _relaysStatus[MAX_RELAY_CNT + 1];
  int _length;
  uint16_t _relayGroups[MAX_RELAY_GROUPS_CNT];
  int _lengthRelayGroups;
public:
  ARelayArr();
  void add(uint8_t pin);
  void del(byte index);
  void clear(void);
  void addRelayGroup(uint16_t rg);
  void clearRelayGroups();
  int getLength();
  int getRelayGroupsLength();
  void printStatus(int index);
  void relayOn(int index);
  void relayOff(int index);
  void relayInvert(int index);
  void relayGroupOn(int gindex);
  void relayGroupOff(int gindex);
  void relayGroupInvert(int gindex);
  void relayGroupOnOrInvert(int gindex);
  void relayGroupOffOrInvert(int gindex);
  void relayGroupNextCombination(int gindex);
  void relaySetStatusAll(uint16_t states); 
  int relayStatusAll(void);
  char * relayStatusAllStr(void);
};

#endif
