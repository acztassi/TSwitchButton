/*
    TSwitchButton 

    Developer:
    Antonio Carlos Z. Tassi

    email: acztassi@gmail.com
    github: https://github.com/acztassi/TSwitchButton

*/



#ifndef SwitchButton
#define SwitchButton

#include <Arduino.h>

#define C_MaxCommands 20
#define C_MaxDebounceTime 20
#define C_MaxTimeToAcceptClick 500
#define C_MaxTimeOut 3000
#define C_TimeBetweenCalls 1000

struct TSwitchCommand;
class TSwitchCommands;

enum TSwitchCommandKind {cNone, cClick, cPress};
enum TSwitchEventTime {tDebounce, tAcceptClick, tlongPress, tTimedOut};
typedef void (*TSwitchCallback)(int ASwitchId, TSwitchCommands ACommands);  //(void*);
typedef void (*TSwitchEvent)(int ASwitchId, TSwitchCommands ACommands, TSwitchCallback &ACallback);


struct TSwitchCommand{ 
  public:
    TSwitchCommandKind Kind;
    int PressedTime = 0;
};

class TSwitchCommands { 
  protected:
    TSwitchCommand FCommands[C_MaxCommands];
  public:
    int Count();
    TSwitchCommand Item(int AIndex);
    TSwitchCommand AtualItem();
    TSwitchCommand operator [](int AIndex);
    bool IsSigleClick();
    bool IsDoubleClick();
    bool IsSinglePress();
    bool IsClickAndPress();
};

class TSwitchCommandsAccess : public TSwitchCommands {
  public:
    void Add(TSwitchCommandKind ACommand, int APressedTime);
    void Clear();
    void SetPressedTime(int APressedTime);
};

class TSwitchButton { 
  private:
    int FSwitchID;  
    TSwitchCallback FCallback = nullptr;
    TSwitchEvent FSwitchEvent = nullptr;
    TSwitchCommandsAccess FCommands;  
    unsigned long FCurrentMillis;   
    unsigned long FBeginClickTime;
    unsigned long FEndClickTime;
    unsigned long FPressedTime;
    unsigned long FBeginLongPressTime;
    unsigned long FTimeLastFinishedCall = 0;
    int FLongPressCurrentTime = -1;
    bool FIsPressing = false;
    bool FIsOnLongPress = false;
    int FLongPressMaxTime = 5000;

    void EndCommand(bool AFireProcedure);
    void RunCommand(TSwitchCommandKind ACommand, int APressedTime) ;
    void BeginPress();
    void EndPress();
    void LongPress();
    void Pressing();
    void Idle();
    void RefreshPressedTime();
    void SetLongPressState(bool AInLongPress);
    bool IsBetweenCalls();
    bool IsOnDebounceTime(unsigned long &ATimeToCheck, unsigned long &ATimeToClear);    
    TSwitchEventTime EventTime(unsigned long AElapsedTime, unsigned int ATimeOutLimit);

  public: 
    TSwitchButton(int ASwitchId, TSwitchEvent ASwitchEvent, int ALongPressMaxTime = 5000);
    void Refresh(bool AState, unsigned long ACurrentMillis);    
};

#endif