#ifndef AButton_h
#define AButton_h
#include "ADeviceHWConfig.h"
#include <GyverButton.h>

enum AButtonClickType {NO_CLICK, SHORT_CLICK, LONG_CLICK, LONGLONG_CLICK};
enum ARelayGroupAction {RG_NO_ACTION, RG_ALL_ON, RG_ALL_OFF, RG_INVERT, RG_NEXT_COMBINATION, RG_ON_OR_INVERT, RG_OFF_OR_INVERT};

/*Structure describes what action to perform on specific Relay Group based on click type (duration) */
typedef struct {
    AButtonClickType click;
    byte relayGroupIndex;
    ARelayGroupAction relayGroupAction; 
} AButtonAction;
  
class AButton  {
private:
    GButton * _button = NULL;
    AButtonClickType _click;
    int _actionsLength;    
    AButtonAction _actions[MAX_BUTTON_ACTIONS_CNT];
    void setClick(AButtonClickType button_click_type);
public:
    AButton(uint8_t pin);
    ~AButton();
    GButton * button();
    AButtonClickType click();
    boolean addButtonAction(AButtonClickType button_click_type, int relay_group_index, ARelayGroupAction relay_group_action);
    int getButtonActionsLength();    
    AButtonAction * getAction(int action_index);
};

class AButtonArr {
private: 
    AButton * _buttons[MAX_BUTTON_CNT];
    int _length;
public:
    AButtonArr();
    ~AButtonArr();
    boolean add(uint8_t pin);
    boolean del(byte index = -1);
    void clear(void);
    int getLength(void);

    AButtonClickType isClicked(byte index = -1);

    void tick(int index);
    boolean isClick(byte index);
    boolean isPress(byte index);
    boolean isRelease(byte index);
    boolean isStep(byte index);
    boolean isHolded(byte index);
    boolean addButtonAction(int button_index, AButtonClickType button_click_type, int relay_group_index, ARelayGroupAction relay_group_action);
    int getButtonActionsLength(int button_index);
    int getButtonActionsLength(void);
    AButtonAction * getAction(int button_index, int action_index);
};

#endif
