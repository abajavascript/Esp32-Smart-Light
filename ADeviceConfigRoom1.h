
const char * deviceConfigDefault = 
      "{"
          //Pins where attached Relays. Should not exceed MAX_RELAY_CNT = 16
          "\"relays\" : [12, 13, 14, 15, 16, 17, 18, 19]," 
          //Configuration of Relay groups. Not more than MAX_RELAY_GROUPS_CNT 32 groups and not more MAX_RELAY_CNT = 16 relays in each group
          "\"relayGroups\" : [[0, 1], [2, 3], [4, 5], [6, 7], [0, 1, 2, 3], [4, 5, 6, 7], [0, 1, 2, 3, 4, 5, 6, 7], [3, 4], [2, 3, 4], [0, 2]]," //array [0..31] of array [0..15]
          //Pins where attached Buttons. Should not exceed MAX_BUTTON_CNT = 16
          "\"buttons\" : [21, 22, 23, 25, 26, 27, 32, 33],"
          //Configure actions on Relay Groups depending on Click type (based on duration Short<1sec, Long=1..3 sec, Very Long>3sec)
          //Read this as "when on button #{param0} occured click with type {param1} on relay group #{param2} do action {param3}"
          //param0 = index of button, 0..buttons.length-1
          //param1 = click type: enum clickType {NO_CLICK=0, SHORT_CLICK=1, LONG_CLICK=2, LONGLONG_CLICK=3};
          //param2 = index of relay group, 0..relayGroups.length-1
          //param3 = action to perform for relay: enum relayGroupAction {RG_NO_ACTION=0, RG_ALL_ON=1, RG_ALL_OFF=2, RG_INVERT=3, RG_NEXT_COMBINATION=4, RG_ON_OR_INVERT=5, RG_OFF_OR_INVERT=6};
          //for any button not more than MAX_BUTTON_ACTIONS_CNT = 8 actions
          "\"buttonActions\" : ["
              "[0, 1, 0, 5], "
              "[0, 2, 7, 5], "
              "[0, 3, 6, 6], "
              "[1, 1, 1, 4], "
              "[1, 2, 8, 5], "
              "[1, 3, 6, 6], "
              "[2, 1, 9, 5], "
              "[2, 2, 4, 5], "
              "[2, 3, 6, 6], "
              "[3, 1, 4, 4], "
              "[3, 2, 4, 5], "
              "[3, 3, 6, 6], "
              "[4, 1, 0, 4], "
              "[4, 2, 0, 5], "
              "[4, 3, 6, 6], "
              "[5, 1, 7, 4], "
              "[5, 2, 8, 5], "
              "[5, 3, 6, 6], "
              "[6, 1, 0, 4], "
              "[6, 2, 0, 5], "
              "[6, 3, 6, 6], "
              "[7, 1, 7, 4], "
              "[7, 2, 8, 5], "
              "[7, 3, 6, 6] "
          "]"
      "}";

