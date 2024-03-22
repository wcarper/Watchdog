
/* This module contains common functions for 
both self mode and compatability operation modes
*/

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <stdint.h>
#include "AS5600.h"
#include <Preferences.h>



extern  int FILSWpin;
extern  int Resetpin;
extern  int Outputpin;
extern AS5600 as5600;  
extern Preferences preferences;
extern bool ErrorState;
extern bool Timer_Complete;
extern int JamDetectionAttempt;


int LastMeasuredPos = 0;
int FilPos = 0;
bool FilamentHasMoved = false;
int TriggerStrikes = 3;
int TriggerCount =0;
float MeasuredDistance = 0;






void TriggerOutput(bool ErrorState){
      
      
          if (preferences.getBool("InvertOut")){
          digitalWrite(Outputpin, !(ErrorState));
        } else {
          digitalWrite(Outputpin, ErrorState);
        }
     
   
  }


bool DetectFilament () {
      if(!digitalRead(FILSWpin)) {
            //Serial.println ("Filament  Detected");
            return true;
          } else {
                return false;
               // Serial.println ("Filament Not Detected");
          }
}

int ReadPosSensor () { // necessary to add some hysteresis to the test as the sensor can drift
  
  int HysteresisFactor = preferences.getInt("Sensitivity");
  
    if ((LastMeasuredPos <= (FilPos + HysteresisFactor)) && (LastMeasuredPos >= (FilPos- HysteresisFactor))) {
          return LastMeasuredPos;
 // } else if (FilPos < LastMeasuredPos)  { //indication that filament is running backwards due to retraction so ignore
   //  return LastMeasuredPos;
   } else {

        return FilPos;
  }
   

}
void DetectMovement () { 
    
    FilPos = as5600.getCumulativePosition();
   
         int HysteresisFactor = preferences.getInt("Sensitivity");
         if (!FilamentHasMoved) {
             if ((LastMeasuredPos <= (FilPos + HysteresisFactor)) && (LastMeasuredPos >= (FilPos- HysteresisFactor))) {
              Serial.println ("Filament Move False");
              FilamentHasMoved = false;
              } else {
                JamDetectionAttempt = 0;
                FilamentHasMoved = true;
              }

         }
         
     }
   
    

 void ResetSensor(){
      LastMeasuredPos = 0;
      ErrorState = false;
      FilamentHasMoved = false;
      Timer_Complete = false;
      TriggerOutput(false);
      char Line1[20] = "Device Reset";
      Display_Screen(Line1);
      as5600.resetPosition(0);
      as5600.resetCumulativePosition(0);
      MeasuredDistance = 0.0;
      TriggerCount = 0;
      JamDetectionAttempt = 0;
      
      void timerRestart(hw_timer_t *timer);
      delay(1000);
 }

 float GetCumlativeDistance(){
  //measures the amount of filment that has been extruded since last reset
    
  float mmPerRotation = preferences.getFloat("RotationMM");

  float CurDistance = ((float)as5600.getCumulativePosition()/4096 *mmPerRotation);
  //this is a work around due to limitations of the getCumlativePosition AS5600 being a 16 bit int
  if (CurDistance > 3000.0) { //this is just before the 16 bit int will error out
      MeasuredDistance = MeasuredDistance + CurDistance; //store what we have measured so far
       as5600.resetCumulativePosition(0); //set the register back to 0

  }
  return abs(MeasuredDistance + CurDistance); //will return the full value
     
 }

      



