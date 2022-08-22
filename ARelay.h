#ifndef ARelay_h
#define ARelay_h
#include "ADeviceHWConfig.h"

/*Reverse controlled relays*/
#define RELAY_ON LOW
#define RELAY_OFF HIGH

/*Class that represents individual relay attached to specific pin*/  
class ARelay {
private:
    uint8_t _PIN = 0;
    boolean _state;

    void setState(boolean state);
public:
    ARelay(uint8_t pin = 0xFF);	
    
    uint8_t getPin();
    void setPin(uint8_t pin); 

    boolean isOn();
    boolean isOff();
    boolean invert();
    boolean on();
    boolean off();
    
};

/*Represent configuration of all Relays attached in device. Up to MAX_RELAY_CNT relays allowed*/
class ARelayArr {
private: 
    ARelay * _relays[MAX_RELAY_CNT];
    int _length;
    char _relaysStatus[MAX_RELAY_CNT + 1];
    uint16_t _relayGroups[MAX_RELAY_GROUPS_CNT];
    int _lengthRelayGroups;
public:
    ARelayArr();
    ~ARelayArr();
    void add(uint8_t pin);
    void del(byte index);
    void clear(void);
    int getLength(void);

    void addRelayGroup(uint16_t relaysInGroup);
    void clearRelayGroups();
    int getRelayGroupsLength();

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
    void printStatus(int index);
    char * relayStatusAllStr(void);
};

#endif
