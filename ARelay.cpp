#include "ARelay.h"
#include <Arduino.h>

// ==================== ARelay Class ====================
ARelay::ARelay(uint8_t pin) {
	setPin(pin);
}

void ARelay::setState(boolean state) {
	_state = state;
	if (_state) {
		digitalWrite(_PIN, RELAY_ON);
	} else {
		digitalWrite(_PIN, RELAY_OFF);
	}
}

uint8_t ARelay::getPin() {
	return _PIN;
}

void ARelay::setPin(uint8_t pin) {
  _PIN = pin;
  if (_PIN < 0xFF) {
    pinMode(_PIN, OUTPUT);
    ARelay::setState(false);
  }
}

boolean ARelay::isOn() {
	return _state;
}

boolean ARelay::isOff() {
	return !isOn();
}

boolean ARelay::invert() {
	return isOn() ? off() : on();
}

boolean ARelay::on() {
	setState(true);
	return isOn();
}

boolean ARelay::off() {
	setState(false);
	return isOn();
}

// ==================== ARelayArr Class ====================
ARelayArr::ARelayArr(){
  _length = 0;
  _lengthRelayGroups = 0;
}

ARelayArr::~ARelayArr(){
	clear();
}

void ARelayArr::add(uint8_t pin) {
	if (_length < MAX_RELAY_CNT) _relays[_length++] = new ARelay(pin);
  Serial.printf("Initialized relay #%i at pin = %i \n", _length - 1, pin);
}

void ARelayArr::del(byte index) {
	if (index < 0 || index >= _length) return;
  delete _relays[index];
  memmove(&_relays[index], &_relays[index+1], (_length - index - 1) * sizeof(ARelay *));
  _length--;
}

void ARelayArr::clear(void) {
  for (int i = 0; i < _length; i++) delete _relays[i];
  _length = 0;
}

int ARelayArr::getLength() {
  return _length;
}

void ARelayArr::addRelayGroup(uint16_t relaysInGroup) {
  //relaysInGroup - Bit representation of relays included in group
  if (_lengthRelayGroups < MAX_RELAY_GROUPS_CNT) _relayGroups[_lengthRelayGroups++] = relaysInGroup;
  //Worth to print relays configuration on BIN format instead of HEX
  Serial.printf("Initialized relay group #%i with value = %x \n", _lengthRelayGroups - 1, relaysInGroup);
}

void ARelayArr::clearRelayGroups() {
  _lengthRelayGroups = 0;
}

int ARelayArr::getRelayGroupsLength() {
  return _lengthRelayGroups;
}

void ARelayArr::relayOn(int index)
{
  if (index >= _length) return;
  _relays[index]->on();
  //printStatus(index);
}

void ARelayArr::relayOff(int index)
{
  if (index >= _length) return;
  _relays[index]->off();
  //printStatus(index);
}

void ARelayArr::relayInvert(int index) {
  if (index >= _length) return;
  _relays[index]->invert();
  //printStatus(index);
}

void ARelayArr::relayGroupOn(int gindex) {
  if (gindex < 0 || gindex >= _lengthRelayGroups) return;
  uint16_t r = _relayGroups[gindex];
  for (int ri = 0; r > 0; ri++, r=r>>1)
    if (r & 1) relayOn(ri);
}

void ARelayArr::relayGroupOff(int gindex) {
  if (gindex < 0 || gindex >= _lengthRelayGroups) return;
  uint16_t r = _relayGroups[gindex];
  for (int ri = 0; r > 0; ri++, r=r>>1)
    if (r & 1) relayOff(ri);
}

void ARelayArr::relayGroupInvert(int gindex) {
  if (gindex < 0 || gindex >= _lengthRelayGroups) return;
  uint16_t r = _relayGroups[gindex];
  for (int ri = 0; r > 0; ri++, r=r>>1)
    if (r & 1) relayInvert(ri);
}

void ARelayArr::relayGroupOnOrInvert(int gindex) {
  if (gindex < 0 || gindex >= _lengthRelayGroups) return;
  int status = relayStatusAll();
  relayGroupOn(gindex);
  if (status == relayStatusAll()) relayGroupInvert(gindex);
}

void ARelayArr::relayGroupOffOrInvert(int gindex) {
  if (gindex < 0 || gindex >= _lengthRelayGroups) return;
  int status = relayStatusAll();
  relayGroupOff(gindex);
  if (status == relayStatusAll()) relayGroupInvert(gindex);
}

void ARelayArr::relayGroupNextCombination(int gindex) {
  if (gindex < 0 || gindex >= _lengthRelayGroups) return;
  uint16_t r = _relayGroups[gindex];
  for (int ri = 0; r > 0; ri++, r=r>>1)
    if (r & 1) {
      if (_relays[ri]->isOff()) {relayOn(ri); break;}
      if (_relays[ri]->isOn()) relayOff(ri);
    }
}

void ARelayArr::relaySetStatusAll(uint16_t states) {
  for (int index = 0; index < _length; index++) {
    if (states & 1) {
      _relays[index]->on();
    } else {
      _relays[index]->off();
    }
    states = states >> 1;
  }
}

int ARelayArr::relayStatusAll(void) {
  int res = 0;
  for (int index = (_length - 1); index >= 0; index--){
    res <<= 1;
    if (_relays[index]->isOn()) res += 1;
  }
  return res;
}

void ARelayArr::printStatus(int index) {
  if (index >= _length) return;
  Serial.printf("Module at pin %d has state %d .\n", _relays[index]->getPin(), _relays[index]->isOn());
}

char * ARelayArr::relayStatusAllStr(void)
{
  for (int index = (_length - 1); index >= 0; index--){
    _relaysStatus[_length - index - 1] = _relays[index]->isOn() ? '1' : '0';
  }
  _relaysStatus[_length] = '\n';
  return _relaysStatus;
}
