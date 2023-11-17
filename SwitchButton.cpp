/*
    TSwitchButton 

    Developer:
    Antonio Carlos Z. Tassi

    email: acztassi@gmail.com
    github: https://github.com/acztassi/TSwitchButton

*/


#include "SwitchButton.h"
#include "Arduino.h"


/* -------------------- IMPLEMENTATION ------------------------- */
/* -------------------- TSwitchCommands ------------------------ */

int TSwitchCommands::Count() { 
    int i;
    for (i = 0; i < C_MaxCommands; i++) 
    { 
        if (FCommands[i].Kind == cNone) 
        { 
        return i; 
        break; 
        } 
    };
    return 20;
};

TSwitchCommand TSwitchCommands::Item(int AIndex)  {
    return FCommands[AIndex];
};

TSwitchCommand TSwitchCommands::AtualItem() {
    for (int i = (C_MaxCommands - 1); i >= 0; i--) 
    { 
        if (FCommands[i].Kind != cNone) 
        { 
            return FCommands[i]; 
            break; 
        } 
    };
    TSwitchCommand CelarReturn;
    CelarReturn.Kind = cNone;
    CelarReturn.PressedTime = 0;
    return CelarReturn;
};

TSwitchCommand TSwitchCommands::operator [](int AIndex)   {
   return Item(AIndex) ;
};

bool TSwitchCommands::IsSigleClick() {
    return (FCommands[0].Kind == cClick && FCommands[1].Kind == cNone) ;           
};

bool TSwitchCommands::IsDoubleClick() {
    return (FCommands[0].Kind == cClick && FCommands[1].Kind == cClick && FCommands[2].Kind == cNone) ;
};

bool TSwitchCommands::IsLongClick() {
    return (FCommands[0].Kind == cLongClick && FCommands[1].Kind == cNone) ;
};

bool TSwitchCommands::IsClickAndLongClick() {
    return (FCommands[0].Kind == cClick && FCommands[1].Kind == cLongClick && FCommands[2].Kind == cNone) ;
};


/* -------------------- IMPLEMENTATION ------------------------- */
/* -------------------- class TSwitchButtonAccess ------------------------ */

void TSwitchCommandsAccess::Add(TSwitchCommandKind ACommand, int APressedTime) { 
    for (int i = 0; i < C_MaxCommands; i++) 
    {        
        if (FCommands[i].Kind == cNone) 
        {
            FCommands[i].Kind = ACommand;
            FCommands[i].PressedTime = APressedTime; 
            break;
        };
    };
};

void TSwitchCommandsAccess::Clear(){ 
    for (int i = 0; i < C_MaxCommands; i++) 
    { 
        FCommands[i].Kind = cNone; 
        FCommands[i].PressedTime = 0; 
    };
};

void TSwitchCommandsAccess::SetPressedTime(int APressedTime) {
    for (int i = (C_MaxCommands - 1); i >= 0; i--) 
    { 
        if (FCommands[i].Kind != cNone) 
        { 
            FCommands[i].PressedTime = APressedTime; 
            break; 
        } 
    };
};

/* -------------------- IMPLEMENTATION ------------------------- */
/* -------------------- class TSwitchButton ------------------------ */

// PRIVATE _______________________________________________________________________________


void TSwitchButton::EndCommand(bool AFireProcedure) {
    if (FCommands.Count() > 0)
    {        
        if (AFireProcedure && FCallback)
        {
            this -> FCallback(FSwitchID, FCommands);
        };
        FCommands.Clear();  
        this -> FCallback = nullptr;
        FBeginClickTime = 0;
        FEndClickTime = 0;
        SetLongClickState(false);
        FTimeLastFinishedCall = FCurrentMillis;
    };    
};

void TSwitchButton::RunCommand(TSwitchCommandKind ACommand, int AClickingTime) {
    if (AClickingTime == 0 || !FFireEventDuringLongClick) 
    { 
        FCommands.Add(ACommand, AClickingTime); Serial.println("add"+String(AClickingTime));
    } else {
        FCommands.SetPressedTime(AClickingTime); Serial.println("set"+String(AClickingTime));
    }
    
    this -> FCallback = nullptr;
    this -> FSwitchEvent(FSwitchID, FCommands, FCallback);
    
    if (!FCallback) //case there is not callback, the command already had finished
    { this -> EndCommand(false);  };
};

void TSwitchButton::BeginClick() {
    if (IsOnDebounceTime(FBeginClickTime, FEndClickTime)) 
    { return; };
    FIsClicking = true;
    FEndClickTime = 0;
};

void TSwitchButton::EndClick() {
    if (IsOnDebounceTime(FEndClickTime, FBeginClickTime)) 
    { return; };

    FIsClicking = false;

    if (FIsOnLongClick)
    {
       LongClick(); 
       SetLongClickState(false);    
    } 
    else 
    {
        switch (EventTime(FPressedTime, C_MaxTimeOut))
        {
            case tDebounce:                              break;
            case tAcceptClick:  RunCommand(cClick, 0);   break;
            case tlongPress:                             break;
            case tTimedOut:     EndCommand(false);       break;
        };
    };    
};

void TSwitchButton::LongClick() {
    SetLongClickState(true);
    //maybe it is setted to only fire long click at end
    if ((FIsClicking && FFireEventDuringLongClick) || (!FIsClicking && !FFireEventDuringLongClick))
    {
        //ensure the time won't be greatest of defined
        int VLongClickTime = min(FCurrentMillis - FBeginLongClickTime, FLongClickMaxTime);
        RunCommand(cLongClick, VLongClickTime);
    };
};

void TSwitchButton::Clicking() {
    RefreshPressedTime();
    switch (EventTime(FPressedTime, FLongClickMaxTime))
    {
        case tDebounce:                             break; 
        case tAcceptClick:                          break;
        case tlongPress:       LongClick();         break;
        case tTimedOut:    /*EndCommand(true);*/    break; //disabled to wait button release
    };
};

void TSwitchButton::Idle() {
    switch (EventTime(FCurrentMillis - FEndClickTime, C_MaxTimeOut))
    {
        case tDebounce:                         break; 
        case tAcceptClick:                      break;
        case tlongPress:    EndCommand(true);   break;
        case tTimedOut:     EndCommand(false);  break;
    };
};

void TSwitchButton::RefreshPressedTime() {
    FPressedTime = FCurrentMillis - FBeginClickTime;
};

void TSwitchButton::SetLongClickState(bool AInLongClick) {
    if (AInLongClick)
    {
        if (!FIsOnLongClick)
        {
            FIsOnLongClick = true;
            FBeginLongClickTime = FCurrentMillis; 
        };        
    } else {
        FIsOnLongClick = false;
    };
};

bool TSwitchButton::IsBetweenCalls() {
    return ((FCurrentMillis - FTimeLastFinishedCall ) < C_TimeBetweenCalls);
};

bool TSwitchButton::IsOnDebounceTime(unsigned long &ATimeToCheck, unsigned long &ATimeToClear) {
    if (ATimeToCheck == 0)
    { 
        ATimeToCheck = FCurrentMillis;  
    };

    if (FCurrentMillis - ATimeToCheck <= C_MaxDebounceTime) 
    { 
        return true; 
    } else {
        RefreshPressedTime(); //refresh pressed time before clear time
        ATimeToClear = 0;
        ATimeToCheck = FCurrentMillis;
        return false;
    };
};

TSwitchEventTime TSwitchButton::EventTime(unsigned long AElapsedTime, unsigned int ATimeOutLimit) {
    if      (AElapsedTime <= C_MaxDebounceTime      )   { return tDebounce;     }
    else if (AElapsedTime <= C_MaxTimeToAcceptClick )   { return tAcceptClick;  }
    else if (AElapsedTime <= ATimeOutLimit          )   { return tlongPress;    }
    else                                                { return tTimedOut;     }    
};


// PUBLIC _______________________________________________________________________________

TSwitchButton::TSwitchButton(int ASwitchId, TSwitchEvent ASwitchEvent, unsigned long ALongClickMaxTime, bool AFireEventDuringLongClick) : 
    FSwitchID(ASwitchId), 
    FSwitchEvent(ASwitchEvent),
    FLongClickMaxTime(ALongClickMaxTime),
    FFireEventDuringLongClick(AFireEventDuringLongClick)  
{       
    
};

void TSwitchButton::Refresh(bool AState, unsigned long ACurrentMillis) {
    //avoid recursion
    //because it would take processment and during long press would fire more than one 
    //event to the same elapsed time
    if (ACurrentMillis == FCurrentMillis) {  
        return;        
    } else {   
        FCurrentMillis = ACurrentMillis;
        if      (IsBetweenCalls())          { return;         }
        else if (AState  && !FIsClicking)   { BeginClick();   }
        else if (!AState &&  FIsClicking)   { EndClick();     }
        else if (AState  &&  FIsClicking)   { Clicking();     }
        else if (!AState && !FIsClicking)   { Idle();         };     
    };
    
};
