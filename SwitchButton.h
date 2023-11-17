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

enum TSwitchCommandKind {cNone, cClick, cLongClick};
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
    bool IsLongClick();
    bool IsClickAndLongClick();
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
    unsigned long FBeginLongClickTime;
    unsigned long FTimeLastFinishedCall = 0;
    bool FIsClicking = false;
    bool FIsOnLongClick = false;
    unsigned long FLongClickMaxTime;    
    bool FFireEventDuringLongClick;

    void EndCommand(bool AFireProcedure);
    void RunCommand(TSwitchCommandKind ACommand, int AClickingTime) ;
    void BeginClick();
    void EndClick();
    void LongClick();
    void Clicking();
    void Idle();
    void RefreshPressedTime();
    void SetLongClickState(bool AInLongClick);
    bool IsBetweenCalls();
    bool IsOnDebounceTime(unsigned long &ATimeToCheck, unsigned long &ATimeToClear);    
    TSwitchEventTime EventTime(unsigned long AElapsedTime, unsigned int ATimeOutLimit);

  public: 
    TSwitchButton(int ASwitchId, TSwitchEvent ASwitchEvent, unsigned long ALongClickMaxTime = 5000, bool AFireEventDuringLongClick = true);
    void Refresh(bool AState, unsigned long ACurrentMillis);    
};

#endif
