#ifndef AButton_h
#define AButton_h
#include <GyverButton.h>
#include <LinkedList.h>

enum AButtonClickType {NO_CLICK, SHORT_CLICK, LONG_CLICK, LONGLONG_CLICK};
enum ARelayGroupAction {RG_NO_ACTION, RG_ALL_ON, RG_ALL_OFF, RG_INVERT, RG_ON_OR_INVERT, RG_OFF_OR_INVERT, RG_NEXT_BIN_COMBINATION, RG_NEXT_SET_COMBINATION};

/*Structure describes what action to perform on specific Relay Group based on click type (duration) */
typedef struct {
    AButtonClickType click;
    byte relayGroupIndex;
    ARelayGroupAction relayGroupAction; 
} AButtonAction;

/*Class that represents individual button attached to specific pin and its actions based on click duration. 
Use Arduino existing class GButton for actual button behaviour*/  
class AButton  {
friend class AButtonArr;  
private:
    GButton * _button = NULL;
    AButtonClickType _click;
    LinkedList<AButtonAction> _actions;

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

/*Represent configuration of all Buttons attached in device. Up to MAX_BUTTON_CNT buttons allowed*/
class AButtonArr {
private: 
    LinkedList<AButton *> _buttons;
public:
    AButtonArr();
    ~AButtonArr();
    boolean add(uint8_t pin);
    boolean del(byte index);
    void clear(void);
    int getLength(void);

    AButtonClickType isClicked(byte index);

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
