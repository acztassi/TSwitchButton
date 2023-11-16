# TSwitchButton
Library for controlling muliple click and long press in Arduino and ESP32


## Features
- Controls debouce time at start, during and after click.
- Supports multiple simple and long clicks.
- Controls long click time (based on defined limit).
- Controls minimum time between commands.
- Uses callbacks to wait for a new click.
- Support up to 20 different commands, by any order and any kind.
- Supports multiple buttons, reusing the same event.


## Using library

### The main class.

```
#include <SwitchButton.h>
TSwitchButton ButtonTeste(1, ButtonEvent);
```
Arguments:
- ButtonEvent - Is a pointer to an event which is invoked on each interaction. 
- "1" - Is te ID of button, used to identify which button is pressed, in case of reuse events for multiple buttons.

## How to start?
It is needed to setup main event! 


### The main event

```
void ButtonEvent(int ASwitchId, TSwitchCommands ACommands, TSwitchCallback &ACallback)
{ };
```
The required event is fired at each click is finished or a long click overtake the limit for a single click.

#### First Argument: "int ASwitchId" 
Only returns the Id of identification of the button.


#### Second Argument: "TSwitchCommands ACommands"
It works like an array and returns all fired commands since the action had started.
Some methods are avaiables:
  
##### Count() 
Count all fired commands.

##### Item(int AIndex) 
Return informations about each item. It stores the kind of command and the time, in case long click.

##### AtualItem() 
Return the last fired item. It stores the kind of command and the time, in case long click.

##### IsSigleClick() 
Check if it is a single click.

##### IsDoubleClick()
Check if it is a double click.

##### IsSinglePress()
Check if it is a single long click.

##### IsClickAndPress() 
Check if it is a single click followed by a single long press.


#### Third Argument: "TSwitchCallback &ACallback"
Is an argument to be setted or not for a callback in case of is desired to wait for more commands.
If inside the event there is no more possibility of wait for more commands, return it as nullptr.
In case of is wanted to wait for more commands, is necessary to define a function for this argument.

A procedure must have all this arguments:
```
void ButtonCallback (int ASwitchId, TSwitchCommands ACommands)
{ };
```
In where "int ASwitchId" is the button ID, and the "TSwitchCommands ACommands" is the same as above.

Here a sample of how to use callback use:


```C++

void ButtonCallback (int ASwitchId, TSwitchCommands ACommands)
{
  Serial.println("the command has finished inside a callback");
};


void ButtonEvent(int ASwitchId, TSwitchCommands ACommands, TSwitchCallback &ACallback)
{  
  if (ACommands.IsSigleClick()) 
  { 
    Serial.println("click"); 
    ACallback =
      [](int ASwitchId, TSwitchCommands ACommands)
      { Serial.println("callback finished with click");  };
  }
  else if (ACommands.IsDoubleClick())
  { 
    Serial.println("dblclick"); 
    ACallback = nullptr; //does not accept new command ... the command already is executed and does not support another stage
  }
  else if (ACommands[0] == cPress && ACommands[1] == cNone) // a single long click
  { 
    Serial.println("long pressing for " + String(ACommands.AtualItem().PressedTime)); 
    ACallback =
      [](int ASwitchId, TSwitchCommands ACommands)
      { Serial.println("callback finished on single long press for " + String(ACommands.AtualItem().PressedTime)); };
  }
  else if (ACommands.IsClickAndPress()) //same of (ACommands.Item(0) == cClick && ACommands.Item(1) == cPress && ACommands.Item(3) == cNone)
  { 
    Serial.println("click + long pressing for " + String(ACommands.AtualItem().PressedTime)); 
    ACallback = ButtonCallback;
  else
  { 
    int i = 0;
    String Text = "interaction: ";
    while (i < C_MaxCommands)
    {  
      switch (ACommands[i].Kind)
      {
      case cClick:
        Text = Text + " - Click";
        break;
      
      case cPress:
        Text = Text + " - Press(" + String(ACommands[i].PressedTime) + "%)";
        break;
      case cNone: 
        break;
      default:
        break;
      }    
      i++;
    }    
    Serial.println(Text); 
    ACallback = ButtonCallback;
  };

};
```

### The second step:
Make the button work!

```C++
void loop()
{
  ButtonTeste.Refresh(digitalRead(8), millis());  
}
```
The refresh function requires two arguments. The first one is the button state, in where true means pressed and false means unpressed. The second one is the atual time based on current millis() funcion. Internally the will be ignored more than one action at same time to avoid processing.



