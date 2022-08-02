#ifndef AButton_h
#define AButton_h
#include "ADeviceHWConfig.h"
#include <GyverButton.h>

enum clickType {NO_CLICK, SHORT_CLICK, LONG_CLICK, LONGLONG_CLICK};
enum relayGroupAction {RG_NO_ACTION, RG_ALL_ON, RG_ALL_OFF, RG_INVERT, RG_NEXT_COMBINATION, RG_ON_OR_INVERT, RG_OFF_OR_INVERT};

typedef struct {
  clickType click;
  byte relayGroupIndex;
  relayGroupAction rgAction; 
} BTNAction;
  
class AButton  {
private:
    GButton * _button = NULL;
    clickType _click;
    int _actionsLength;    
    BTNAction _actions[MAX_BUTTON_ACTIONS_CNT];
public:
    AButton(uint8_t pin);
    ~AButton();
    GButton * btn();
    clickType click();
    void setClick(clickType ct);
    boolean addButtonAction(int btn_click_type, int relay_group_idx, int relay_action);
    int getButtonActionsLength();    
    BTNAction * getAction(int action_idx);
};

class AButtonArr {
private: 
    AButton * _buttons[MAX_BUTTON_CNT];
    int _length;
public:
    AButtonArr();
    void add(uint8_t pin);
    void del(byte index = -1);
    void clear(void);
    int getLength(void);

    clickType isClicked(byte index = -1);

    void tick(int index);
    boolean isClick(byte index = -1);
    boolean isPress(byte index = -1);
    boolean isRelease(byte index = -1);
    boolean isStep(byte index = -1);
    boolean isHolded(byte index = -1);
    boolean addButtonAction(int btn_idx, int btn_click_type, int relay_group_idx, int relay_action);
    int getButtonActionsLength(int btn_idx);
    int getButtonActionsLength(void);
    BTNAction * getAction(int btn_idx, int action_idx);
};

#endif
