#include "AButton.h"
#include "GyverButton.h"
#include <Arduino.h>

AButton::AButton(uint8_t pin){
  _button = new GButton(pin);
//  _button->setTickMode(true);
  _button->setTimeout(1000);//more is long click
  _button->setStepTimeout(2000);//more than 1000 + 2000 long long click
  _button->resetStates();
  _actionsLength = 0;
}

AButton::~AButton(){
  if (_button != NULL) delete _button;
}

GButton * AButton::btn(){
  return _button;
}

clickType AButton::click(){
  return _click;
}

void AButton::setClick(clickType ct){
  _click = ct;
}

boolean AButton::addButtonAction(int btn_click_type, int relay_group_idx, int relay_action) {
  if (_actionsLength >= MAX_BUTTON_ACTIONS_CNT) return false;
  _actions[_actionsLength].click = (clickType)btn_click_type;
  _actions[_actionsLength].relayGroupIndex = relay_group_idx;
  _actions[_actionsLength].rgAction = (relayGroupAction)relay_action;
  _actionsLength++;
  return true;
}

int AButton::getButtonActionsLength() {
  return _actionsLength;
}

BTNAction * AButton::getAction(int action_idx){
  return &_actions[action_idx];
}

// ==================== CONSTRUCTOR ====================
AButtonArr::AButtonArr(){
	_length = 0;
}

int AButtonArr::getLength() {
  return _length;
}

void AButtonArr::add(uint8_t pin) {
	if (_length >= MAX_BUTTON_CNT) return;
  AButton * btn = new AButton(pin);
	_buttons[_length++] = btn;
  Serial.printf("Initialized button #%i at pin = %i \n", _length - 1, pin);
}

void AButtonArr::del(byte index) {
	if (index < 0 || index >= _length) return;
	delete _buttons[index];
	memmove(&_buttons[index], &_buttons[index+1], (_length - index - 1) * sizeof(AButton *));
	_length--;
}

void AButtonArr::clear(void) {
  for (int i = 0; i < _length; i++) delete _buttons[i];
  _length = 0;
}

clickType AButtonArr::isClicked(byte index) {
  if (index >= _length || index < 0) return NO_CLICK;
  _buttons[index]->btn()->tick();
  if (_buttons[index]->btn()->isPress()) {
    _buttons[index]->setClick(NO_CLICK);
    //Serial.printf("Button #%d - press\n", index);
  }
  if (_buttons[index]->btn()->isClick()) {
    _buttons[index]->setClick(SHORT_CLICK);
    //Serial.printf("Button #%d - click\n", index);
  }
  if (_buttons[index]->btn()->isHolded()) {
    _buttons[index]->setClick(LONG_CLICK);
    //Serial.printf("Button #%d - holded\n", index);
  }
  if (_buttons[index]->btn()->isStep()) {
    _buttons[index]->setClick(LONGLONG_CLICK);
    //Serial.printf("Button #%d - step\n", index);
  }
  if (_buttons[index]->btn()->isRelease()) {
    //Serial.printf("Button #%d - release\n", index);
    return _buttons[index]->click();
  }
  return NO_CLICK;  
}

void AButtonArr::tick(int index) {
  if (index >= _length || index < 0) return;
  _buttons[index]->btn()->tick();
}

boolean AButtonArr::isClick(byte index) {
  if (index >= _length || index < 0) return false;
  return _buttons[index]->btn()->isClick();
}

boolean AButtonArr::isPress(byte index) {
  if (index >= _length || index < 0) return false;
  return _buttons[index]->btn()->isPress();
}

boolean AButtonArr::isRelease(byte index) {
  if (index >= _length || index < 0) return false;
  return _buttons[index]->btn()->isRelease();
}

boolean AButtonArr::isStep(byte index) {
  if (index >= _length || index < 0) return false;
  return _buttons[index]->btn()->isStep();
}

boolean AButtonArr::isHolded(byte index) {
  if (index >= _length || index < 0) return false;
  return _buttons[index]->btn()->isHolded();
}

boolean AButtonArr::addButtonAction(int btn_idx, int btn_click_type, int relay_group_idx, int relay_action) {
  if (btn_idx >= _length || btn_idx < 0) return false;
  Serial.printf("Added acction for button %d when action %d then for Relay Group %d do %d\n", btn_idx, btn_click_type, relay_group_idx, relay_action);
  return _buttons[btn_idx]->addButtonAction(btn_click_type, relay_group_idx, relay_action);
}

int AButtonArr::getButtonActionsLength(int btn_idx) {
  if (btn_idx >= _length || btn_idx < 0) return 0;
  return _buttons[btn_idx]->getButtonActionsLength();
}

int AButtonArr::getButtonActionsLength(void) {
  int res = 0;
  for (int i = 0; i < _length; i++)
    res += _buttons[i]->getButtonActionsLength();
  return res;
}

BTNAction * AButtonArr::getAction(int btn_idx, int action_idx) {
  if (btn_idx >= _length || btn_idx < 0) return NULL;
  if (action_idx >= _buttons[btn_idx]->getButtonActionsLength() || action_idx < 0) return NULL;
  return _buttons[btn_idx]->getAction(action_idx);
}
