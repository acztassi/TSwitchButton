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
/* -------------------- TSwitchConfig ------------------------ */

TSwitchConfig::TSwitchConfig(unsigned int ADebounceTime, unsigned int AMaxTimeToAcceptClick, unsigned int ATimeBetweenFinishedClicks, unsigned int ALongClickMaxTime, unsigned int ATimeOut, bool AFireEventDuringLongClick)
: DebounceTime(ADebounceTime), MaxTimeToAcceptClick(AMaxTimeToAcceptClick), TimeBetweenFinishedClicks(ATimeBetweenFinishedClicks), LongClickMaxTime(ALongClickMaxTime), TimeOut(ATimeOut), FireEventDuringLongClick(AFireEventDuringLongClick)
{
}

/* -------------------- IMPLEMENTATION ------------------------- */
/* -------------------- TSwitchCommands ------------------------ */

int TSwitchCommands::Count() {
    return FLastIndex + 1;
}

TSwitchCommand TSwitchCommands::Item(int AIndex)  {
    if (AIndex >= 0 && AIndex < C_MaxCommands)
        return FCommands[AIndex];    
    else
        return TSwitchCommand();
}

TSwitchCommand TSwitchCommands::AtualItem() {
    if (FLastIndex < (C_MaxCommands - 1))
        return FCommands[FLastIndex];
    else 
        return TSwitchCommand();
}

TSwitchCommand TSwitchCommands::operator [](int AIndex)   {
   return Item(AIndex) ;
}

bool TSwitchCommands::IsSigleClick() {
    return (FCommands[0].Kind == cClick && FCommands[1].Kind == cNone) ;           
}

bool TSwitchCommands::IsDoubleClick() {
    return (FCommands[0].Kind == cClick && FCommands[1].Kind == cClick && FCommands[2].Kind == cNone) ;
}

bool TSwitchCommands::IsLongClick() {
    return (FCommands[0].Kind == cLongClick && FCommands[1].Kind == cNone) ;
}

bool TSwitchCommands::IsClickAndLongClick() {
    return (FCommands[0].Kind == cClick && FCommands[1].Kind == cLongClick && FCommands[2].Kind == cNone) ;
}


/* -------------------- IMPLEMENTATION ------------------------- */
/* -------------------- class TSwitchButtonAccess ------------------------ */

void TSwitchCommandsAccess::Add(TSwitchCommandKind ACommand, int APressedTime) { 
    if (FLastIndex < (C_MaxCommands - 1))
    {
        FLastIndex++;
        FCommands[FLastIndex].Kind = ACommand;
        FCommands[FLastIndex].PressedTime = APressedTime; 
    }
}

void TSwitchCommandsAccess::Clear(){ 
    for (int i = 0; i < C_MaxCommands; i++) 
    { 
        FCommands[i].Kind = cNone; 
        FCommands[i].PressedTime = 0; 
    }
    FLastIndex = -1;
}

void TSwitchCommandsAccess::SetPressedTime(int APressedTime) {
    if (FLastIndex > -1)
    { 
        FCommands[FLastIndex].PressedTime = APressedTime; 
    }
}

/* -------------------- IMPLEMENTATION ------------------------- */
/* -------------------- class TSwitchButton ------------------------ */

// PRIVATE _______________________________________________________________________________


void TSwitchButton::EndCommand(bool AFireProcedure) {
    if (FCommands.Count() > 0)
    {        
        if (AFireProcedure && FCallback)
            this -> FCallback(FSwitchID, FCommands);
        
        FCommands.Clear();  
        this -> FCallback = nullptr;
        FBeginClickTime = 0;
        FEndClickTime = 0;
        SetLongClickState(false);
        FTimeLastFinishedCall = FCurrentMillis;
    }
}

void TSwitchButton::RunCommand(TSwitchCommandKind ACommand, int AClickingTime) {
    if (AClickingTime == 0 || !FConfig.FireEventDuringLongClick)     
        FCommands.Add(ACommand, AClickingTime); 
    else 
        FCommands.SetPressedTime(AClickingTime);    
    
    this -> FCallback = nullptr;
    this -> FSwitchEvent(FSwitchID, FCommands, FCallback);
    
     //case there is not callback, the command already had finished
     //or in case of commands count has reached the limit, fires the end
    if (!FCallback)
        this -> EndCommand(false);
    else if (FCommands.Count() == C_MaxCommands)
        this -> EndCommand(true);
}

void TSwitchButton::BeginClick() {
    if (IsOnDebounceTime(FBeginClickTime, FEndClickTime)) 
        return; 
    FIsClicking = true;
    FEndClickTime = 0;
}

void TSwitchButton::EndClick() {
    if (IsOnDebounceTime(FEndClickTime, FBeginClickTime)) 
        return;

    FIsClicking = false;

    if (FIsOnLongClick)
    {
       LongClick(); 
       SetLongClickState(false);    
    } 
    else 
    {
        switch (EventTime(FPressedTime, FConfig.TimeOut))
        {
            case tDebounce:                              break;
            case tAcceptClick:  RunCommand(cClick, 0);   break;
            case tlongPress:                             break;
            case tTimedOut:     EndCommand(false);       break;
        }
    } 
}

void TSwitchButton::LongClick() {
    SetLongClickState(true);
    //maybe it is setted to only fire long click at end
    if ((FIsClicking && FConfig.FireEventDuringLongClick) || (!FIsClicking && !FConfig.FireEventDuringLongClick))
    {
        //ensure the time won't be greatest of defined
        int VLongClickTime = min(FCurrentMillis - FBeginLongClickTime, FConfig.LongClickMaxTime);
        RunCommand(cLongClick, VLongClickTime);
    }
}

void TSwitchButton::Clicking() {
    RefreshPressedTime();
    switch (EventTime(FPressedTime, FConfig.LongClickMaxTime))
    {
        case tDebounce:                             break; 
        case tAcceptClick:                          break;
        case tlongPress:       LongClick();         break;
        case tTimedOut:    /*EndCommand(true);*/    break; //disabled to wait button release
    }
}

void TSwitchButton::Idle() {
    switch (EventTime(FCurrentMillis - FEndClickTime, FConfig.TimeOut))
    {
        case tDebounce:                         break; 
        case tAcceptClick:                      break;
        case tlongPress:    EndCommand(true);   break;
        case tTimedOut:     EndCommand(false);  break;
    }
}

void TSwitchButton::RefreshPressedTime() {
    FPressedTime = FCurrentMillis - FBeginClickTime;
}

void TSwitchButton::SetLongClickState(bool AInLongClick) {
    if (AInLongClick)
    {
        if (!FIsOnLongClick)
        {
            FIsOnLongClick = true;
            FBeginLongClickTime = FCurrentMillis; 
        }       
    } else 
        FIsOnLongClick = false;
}

bool TSwitchButton::IsBetweenCalls() {
    return ((FCurrentMillis - FTimeLastFinishedCall ) < FConfig.TimeBetweenFinishedClicks);
}

bool TSwitchButton::IsOnDebounceTime(unsigned long &ATimeToCheck, unsigned long &ATimeToClear) {
    if (ATimeToCheck == 0) 
        ATimeToCheck = FCurrentMillis;  

    if (FCurrentMillis - ATimeToCheck <= FConfig.DebounceTime) 
    { 
        return true; 
    } else {
        RefreshPressedTime(); //refresh pressed time before clear time
        ATimeToClear = 0;
        ATimeToCheck = FCurrentMillis;
        return false;
    }
}

TSwitchEventTime TSwitchButton::EventTime(unsigned long AElapsedTime, unsigned int ATimeOutLimit) {
    if      (AElapsedTime <= FConfig.DebounceTime           )   { return tDebounce;     }
    else if (AElapsedTime <= FConfig.MaxTimeToAcceptClick   )   { return tAcceptClick;  }
    else if (AElapsedTime <= ATimeOutLimit                  )   { return tlongPress;    }
    else                                                        { return tTimedOut;     }    
}


// PUBLIC _______________________________________________________________________________

TSwitchButton::TSwitchButton(int ASwitchId, TSwitchEvent ASwitchEvent, TSwitchConfig AConfig) : 
    FSwitchID(ASwitchId), 
    FSwitchEvent(ASwitchEvent),
    FConfig(AConfig)
{       
    
}


void TSwitchButton::Refresh(bool AState, unsigned long ACurrentMillis)
{
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
    }
}


