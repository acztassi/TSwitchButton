#include <Arduino.h>
#include <SwitchButton.h>




void ButtonCallback (int ASwitchId, TSwitchCommands ACommands)
{
  Serial.println("undefined callback");
};


void ButtonEvent(int ASwitchId, TSwitchCommands ACommands, TSwitchCallback &ACallback)
{  
  if (ACommands.IsSigleClick()) //same of (ACommands.Item(0) == cClick && ACommands.Item(1) == cNone)
  { 
    Serial.println("click"); 
    ACallback = [](int ASwitchId, TSwitchCommands ACommands) { Serial.println("callback finished with click");  };
  }
  else if (ACommands.IsDoubleClick()) //same of (ACommands.Item(0) == cClick && ACommands.Item(1) == cClick && ACommands.Item(3) == cNone)
  { 
    Serial.println("dblclick"); 
    ACallback = [](int ASwitchId, TSwitchCommands ACommands) { Serial.println("callback finished with dblclick");  };
   // ACallback = nullptr; //does not accept new command ... the command already is executed and does not support another stage
  }
  else if (ACommands.IsSinglePress())  //same of (ACommands.Item(0) == cPress && ACommands.Item(1) == cNone)
  { 
    Serial.println("long pressing for " + String(ACommands.AtualItem().PressedTime)); 
    ACallback = [](int ASwitchId, TSwitchCommands ACommands) { Serial.println("callback finished on single long press for " + String(ACommands.AtualItem().PressedTime));  };
  }
  else if (ACommands.IsClickAndPress()) //same of (ACommands.Item(0) == cClick && ACommands.Item(1) == cPress && ACommands.Item(3) == cNone)
  { 
    Serial.println("click + long pressing for " + String(ACommands.AtualItem().PressedTime)); 
    ACallback = [](int ASwitchId, TSwitchCommands ACommands) { Serial.println("callback finished on single click + long press for " + String(ACommands.AtualItem().PressedTime));  };
  }
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

TSwitchButton ButtonTeste(1, ButtonEvent);

int (* Teste)[5];
int Tes[5] = {1,2,3,4,5};

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);  
  pinMode(8, INPUT_PULLUP);

}

void loop() {

  ButtonTeste.Refresh(!digitalRead(8), millis());

  if (Serial.available() > 0)
    {
      Serial.println(Serial.readString());
    };

}




