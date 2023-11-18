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
// #define C_MaxDebounceTime 20
// #define C_MaxTimeToAcceptClick 500
// #define C_MaxTimeOut 3000
// #define C_TimeBetweenCalls 1000

struct TSwitchCommand;
class TSwitchCommands;

enum TSwitchCommandKind {cNone, cClick, cLongClick};
enum TSwitchEventTime {tDebounce, tAcceptClick, tlongPress, tTimedOut};
typedef void (*TSwitchCallback)(int ASwitchId, TSwitchCommands ACommands);  //(void*);
typedef void (*TSwitchEvent)(int ASwitchId, TSwitchCommands ACommands, TSwitchCallback &ACallback);

struct TSwitchConfig{
  public:
    TSwitchConfig(unsigned int ADebounceTime = 20, unsigned int AMaxTimeToAcceptClick = 500, unsigned int ATimeBetweenFinishedClicks = 1000, unsigned int ALongClickMaxTime = 5000, unsigned int ATimeOut = 2000, bool AFireEventDuringLongClick = true);
    //Time in milliseconds, time of rejection for unwanted interefence
    unsigned int DebounceTime = 20;
    //Time in milliseconds, time between button press and release to accept it as single click
    unsigned int MaxTimeToAcceptClick = 500;
    //Time in milliseconds, time to reject new operation after an operation is finished
    unsigned int TimeBetweenFinishedClicks = 1000;
    //Time in milliseconds, time between long click is identified and it's end. After this, the counter stops counting and only release the event after button has been released.
    unsigned int LongClickMaxTime = 5000;
    //Time in milliseconds, time to cancel the operation. It starts counting from beginin of press. Important: it must be greater than MaxTimeToAcceptClick and TimeBetweenFinishedClicks
    unsigned int TimeOut = 2000;
    //Sets how event will be fired in case of long click. If will be fired each time change (many fires) or fired only at the end (showing the traveled time)
    bool FireEventDuringLongClick = true;
};

struct TSwitchCommand{ 
  public:
    TSwitchCommandKind Kind;
    int PressedTime = 0;
};

class TSwitchCommands { 
  protected:
    TSwitchCommand FCommands[C_MaxCommands];
    int FLastIndex = 0;
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
    TSwitchConfig FConfig;
    unsigned long FCurrentMillis;   
    unsigned long FBeginClickTime;
    unsigned long FEndClickTime;
    unsigned long FPressedTime;
    unsigned long FBeginLongClickTime;
    unsigned long FTimeLastFinishedCall = 0;
    bool FIsClicking = false;
    bool FIsOnLongClick = false;
    // unsigned long FLongClickMaxTime;    
    // bool FFireEventDuringLongClick;

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
    TSwitchButton(int ASwitchId, TSwitchEvent ASwitchEvent, TSwitchConfig AConfig);
    void Refresh(bool AState, unsigned long ACurrentMillis);    
};

#endif
