#ifndef ASchedule_h
#define ASchedule_h
#include <Arduino.h>
#include <LinkedList.h>

/*Class that represents individual Schedule configuration*/  
class ASchedule {
private:
    time_t t_prev = 0, t_now = 0;
public:
    String _description;
    boolean _active; //off = false, on = true
    String _dayOfWeek; //Example '0111110' (sun, mon, .., sat)
    int _when; //exactTime = 0, beforeSunrise = 1, afterSunrise = 2, beforeSunshine = 3, afterSunshine = 4
    int _hour; //0...23
    int _minute; //0..59
    String _action; //'??11??00' off = 0, on = 1, skip = ?, inver = !
    //repeatable or number of repeats: like infinite repeat every hour 
public:
    ASchedule();	
    
    boolean isTimeElapsed(time_t t);
    boolean process();
    boolean doAction(void);
};

/*Represent configuration of all Schedules configured in device. */
class AScheduleArr {
private: 
    LinkedList<ASchedule *> schedules;
public:
    AScheduleArr();
    ~AScheduleArr();
    void add(ASchedule * schedule);
    void del(byte index);
    void clear(void);
    int getLength(void);
    boolean process();
};

#endif
