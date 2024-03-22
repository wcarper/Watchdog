


/* This module contains helper functions related to
the operation of OLED display
*/

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <stdint.h>
#include "AS5600.h"
#include <Preferences.h>


//declare global var
extern Adafruit_SSD1306 display;
extern AS5600 as5600;  
extern Preferences preferences;
extern IPAddress apIP;


void Display_Screen(char Line1[20], char Line2[20],char Line3[20],char Line4[20])
{    display.clearDisplay();
      if (preferences.getBool("InvertScn")){display.setRotation(2); }
      display.setTextSize(1);
     display.setTextColor(SSD1306_WHITE);
     display.setCursor(0,0);
     display.println(Line1);
     display.println(Line2);
     display.println(Line3);
     display.println(Line4);
     display.setCursor(0,0);
     display.display(); // actually display all of the above
}

void Display_Screen(char Line1[20]){
  char Line2[20] = "";
  char Line3[20] = "";
  char Line4[20] = "";
  Display_Screen(Line1, Line2,Line3,Line4);
}

void Display_Screen(char Line1[20], char Line2[20]){
  char Line3[20] = "";
  char Line4[20] = "";
  Display_Screen(Line1, Line2,Line3,Line4);
}

void Display_Screen(char Line1[20], char Line2[20],  char Line3[20]){
  char Line4[20] = "";
  Display_Screen(Line1, Line2,Line3,Line4);
}

void Display_Sensors(int FILSWpin, int Resetpin){
    //if in config state we will display the sensor values to the screen
    //and will set the ESP to AP mode so that user can configure device
     char FilSwitchState[] = "                    ";
     char ResetSwitchState[] = "                    ";
     char PositionValue[] ="    ";

    if(!digitalRead(Resetpin)) {
        strcpy(ResetSwitchState,"Reset SW: Pressed");
    } else {
        strcpy(ResetSwitchState,"Reset SW: Not Press");
    }

     if(!digitalRead(FILSWpin)) {
        strcpy(FilSwitchState,"Filament: Present"); 
    } else {
        strcpy(FilSwitchState,"Filament: Missing"); 
    }
    char PosState[20];
    int FilPos = as5600.getCumulativePosition();
    sprintf(PosState, "Position: %d", FilPos);

    char IPAddress[20] = "";
    sprintf(IPAddress, "http://%s",  apIP.toString());

    Display_Screen(ResetSwitchState, FilSwitchState, PosState, IPAddress);

}
    

