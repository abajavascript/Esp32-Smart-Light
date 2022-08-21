#include "AButton.h"
#include "GyverButton.h"
#include <Arduino.h>

// ==================== AButton Class ====================
AButton::AButton(uint8_t pin){
  _button = new GButton(pin);
  //_button->setTickMode(true); // lets have manual tick control for a moment
  _button->setTimeout(1000);//click shorter that 1000 ms considered as regular click, longer than 1000 ms is long-click (1000 - 3000 ms) or long-long-click (> 3000 ms)
  _button->setStepTimeout(2000);//clicks longer  than 1000 + 2000 ms considered as long-long-click (> 3000 ms)
  _button->resetStates();
  _actionsLength = 0;
}

AButton::~AButton(){
  if (_button != NULL) delete _button;
}

GButton * AButton::button(){
  return _button;
}

AButtonClickType AButton::click(){
  return _click;
}

void AButton::setClick(AButtonClickType button_click_type){
  _click = button_click_type;
}

boolean AButton::addButtonAction(AButtonClickType button_click_type, int relay_group_index, int relay_group_action) {
  if (_actionsLength >= MAX_BUTTON_ACTIONS_CNT) return false;
  _actions[_actionsLength].click = (AButtonClickType)button_click_type;
  _actions[_actionsLength].relayGroupIndex = relay_group_index;
  _actions[_actionsLength].relayGroupAction = (ARelayGroupAction)relay_group_action;
  _actionsLength++;
  return true;
}

int AButton::getButtonActionsLength() {
  return _actionsLength;
}

AButtonAction * AButton::getAction(int action_index){
  return &_actions[action_index];
}

// ==================== AButtonArr Class ====================
AButtonArr::AButtonArr(){
	_length = 0;
}

AButtonArr::~AButtonArr(){
	clear();
}

int AButtonArr::getLength() {
  return _length;
}

boolean AButtonArr::add(uint8_t pin) {
	if (_length >= MAX_BUTTON_CNT) return false;
  AButton * button = new AButton(pin);
	_buttons[_length] = button;
  _length++;
  Serial.printf("Initialized button #%i at pin = %i \n", _length - 1, pin);
  return true;
}

boolean AButtonArr::del(byte index) {
	if (index < 0 || index >= _length) return false;
	delete _buttons[index];
	memmove(&_buttons[index], &_buttons[index+1], (_length - index - 1) * sizeof(AButton *));
	_length--;
  return true;
}

void AButtonArr::clear(void) {
  for (int i = 0; i < _length; i++) delete _buttons[i];
  _length = 0;
}

AButtonClickType AButtonArr::isClicked(byte index) {
  //Dont use serial in case code is invoced in interrupt handler
	if (index < 0 || index >= _length) return NO_CLICK;
  _buttons[index]->button()->tick();
  if (_buttons[index]->button()->isPress()) {
    _buttons[index]->setClick(NO_CLICK);
    //Serial.printf("Button #%d - press\n", index);
  }
  if (_buttons[index]->button()->isClick()) {
    _buttons[index]->setClick(SHORT_CLICK);
    //Serial.printf("Button #%d - click\n", index);
  }
  if (_buttons[index]->button()->isHolded()) {
    _buttons[index]->setClick(LONG_CLICK);
    //Serial.printf("Button #%d - holded\n", index);
  }
  if (_buttons[index]->button()->isStep()) {
    _buttons[index]->setClick(LONGLONG_CLICK);
    //Serial.printf("Button #%d - step\n", index);
  }
  if (_buttons[index]->button()->isRelease()) {
    //Serial.printf("Button #%d - release\n", index);
    return _buttons[index]->click();
  }
  return NO_CLICK;  
}

void AButtonArr::tick(int index) {
	if (index < 0 || index >= _length) return;
  _buttons[index]->button()->tick();
}

boolean AButtonArr::isClick(byte index) {
	if (index < 0 || index >= _length) return false;
  return _buttons[index]->button()->isClick();
}

boolean AButtonArr::isPress(byte index) {
	if (index < 0 || index >= _length) return false;
  return _buttons[index]->button()->isPress();
}

boolean AButtonArr::isRelease(byte index) {
	if (index < 0 || index >= _length) return false;
  return _buttons[index]->button()->isRelease();
}

boolean AButtonArr::isStep(byte index) {
	if (index < 0 || index >= _length) return false;
  return _buttons[index]->button()->isStep();
}

boolean AButtonArr::isHolded(byte index) {
	if (index < 0 || index >= _length) return false;
  return _buttons[index]->button()->isHolded();
}

boolean AButtonArr::addButtonAction(int button_index, AButtonClickType button_click_type, int relay_group_index, ARelayGroupAction relay_group_action) {
  if (button_index < 0 || button_index >= _length) return false;
  Serial.printf("Added acction for button %d when action %d then for Relay Group %d do %d\n", button_index, button_click_type, relay_group_index, relay_group_action);
  return _buttons[button_index]->addButtonAction(button_click_type, relay_group_index, relay_group_action);
}

int AButtonArr::getButtonActionsLength(int button_index) {
  if (button_index < 0 || button_index >= _length) return 0;
  return _buttons[button_index]->getButtonActionsLength();
}

int AButtonArr::getButtonActionsLength(void) {
  int res = 0;
  for (int i = 0; i < _length; i++)
    res += _buttons[i]->getButtonActionsLength();
  return res;
}

AButtonAction * AButtonArr::getAction(int button_index, int action_index) {
  if (button_index >= _length || button_index < 0) return NULL;
  if (action_index >= _buttons[button_index]->getButtonActionsLength() || action_index < 0) return NULL;
  return _buttons[button_index]->getAction(action_index);
}
