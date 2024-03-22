#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "AS5600.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "SPIFFS.h"
#include <Preferences.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <DNSServer.h>



//code is distributed across multiple modules to improve maintainability
#include "OLED.h"
#include "configuration.h"
#include "CommonFunctions.h"
#include "Notification.h"


# define SDApin 36 //used  for I2C as this chip has no default pin
# define SCLpin 34  //used  for I2C as this chip has no default pin
# define  DIRpin 21 //AS5600 Dir 
//must declare pins as they are accessed from multiple modules
int  FILSWpin = 13; //Filament present switch
int  Resetpin = 10; //Reset switch 
int  Outputpin = 8; //output state

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
AS5600 as5600(&Wire);   
IPAddress apIP(192, 168, 4, 1);

Preferences preferences;

AsyncWebServer server(80);
const char* ssid     = "W3RIoT";
const char* password = "Watchdog";
bool ConfigMode = false;
bool ErrorState = false;
hw_timer_t *Timer0_Cfg = NULL;
bool Timer_Complete = false;
char FilDistanceMM[20] = "";

HTTPClient http;
IPAddress IP;
const byte DNS_PORT = 53;

int MinMoveDistance = 100; //in mm before monitoring for a jam
long FilDistance = 0;
char Line1[20] = "";
int JamDetectionAttempt = 0;

void DisplayStopScreen(){
  if ( JamDetectionAttempt > 0) { 
      if ( !FilamentHasMoved  &&  (FilDistance > preferences.getInt("Threshold")) && !ErrorState ) {
            char Line2[20] = "";
            char Line3[20] = "Filament Stopped";
            //char Line4[20] = "Self Detect Mode";
            Display_Screen(Line1, Line2, Line3, FilDistanceMM);
            
            if(!preferences.getBool("NotifyOnly")) {
              if(preferences.getBool("SelfMode")) {TriggerOutput(true);}
              }

            if(preferences.getBool("SendNote") && !ErrorState) {
              SendNotification (preferences.getString("FilStop") + " LastMeasuredPos: " +  String(LastMeasuredPos) + " FilPos: " +  String(FilPos));
              }
              ErrorState = true;
              JamDetectionAttempt = 0;
            
    } 
  }
    JamDetectionAttempt++;
    Serial.println(JamDetectionAttempt);
}

void IRAM_ATTR Timer0_ISR()
{      
        Timer_Complete = true;
     
        Serial.println("timer completed");
     
}

void setup() {

  
  delay(5000); //gives user a sec to press the reset button if config mode is needed
  Serial.begin(115200);
  Serial.print("Setup Watchdog…"); 

  Wire.begin (SDApin, SCLpin);  
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
 
  //initialize preferences
  preferences.begin("watchdog", false); 
  


  //if this is the first time opening the appspace then will create all name-val pairs with default values
  if (preferences.getBool("default", true)){
      preferences.putString("PName", "default printer");
      preferences.putBool("default", false); 
      preferences.putBool("InvertOut", false); 
      preferences.putBool("InvertScn", false);
      preferences.putBool("MonFil", true);
      preferences.putBool("SelfMode", true);
      preferences.putInt("DelaySec", 60);
      preferences.putFloat("RotationMM", 34.4);
      preferences.putInt("Sensitivity", 20);
      preferences.putInt("Threshold", 100);
      preferences.putInt("RunLen", 7);
      preferences.putInt("PulseWidth", 100);
      preferences.putBool("SendNote", true);
      preferences.putBool("NotifyOnly", true);
      preferences.putString("NoteURL", "https://api.callmebot.com/whatsapp.php?");
      preferences.putString("NotePhone", "1NNNNNNNNN for US");
      preferences.putString("NoteAPIKey", "Your Key");
      preferences.putString("FilRunout", "Watchdog reports filament runout");
      preferences.putString("FilStop", "Watchdog reports filament has stopped");
      preferences.putString("SSID", "Your SSID");
      preferences.putString("PWD", "Your SSID PWD");
 } 
     //initialize the OLED Display
    Serial.println("OLED test");
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
    Serial.println("OLED begun");
    
    char Line1[20] = "W3RIOT Watchdog";
    Display_Screen(Line1);

     //initialize the AS5600 position sensor
        Serial.println(__FILE__);
        Serial.print("AS5600_LIB_VERSION: ");
        Serial.println(AS5600_LIB_VERSION);
        int b = as5600.isConnected();
        Serial.print("Connect: ");
        Serial.println(b);
        as5600.begin(DIRpin);  //  set direction pin.
        as5600.setDirection(AS5600_CLOCK_WISE);  //  default, just be explicit.
        Serial.println(as5600.getAddress());
        as5600.resetPosition(0);
        as5600.resetCumulativePosition(0); //perform a reset at startup


    //initialize the sensor pins

    pinMode(FILSWpin, INPUT_PULLUP);
    pinMode(Resetpin, INPUT_PULLUP);
    pinMode(Outputpin, OUTPUT);

 if(!digitalRead(Resetpin)) {
   //holding reset at startup will cause device to enter config mode
    ConfigMode = true;
   //configure web interface for accessing configuration preferences
        
    SetupWiFi();
  } 
 
    Timer0_Cfg = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, preferences.getInt("DelaySec")*1000000, true);
    timerAlarmEnable(Timer0_Cfg);

}



 void loop() {
    
    if(!digitalRead(Resetpin) && !ConfigMode) {
      //reset switch pressed
      ResetSensor();
    }
    
    FilDistance = GetCumlativeDistance();
    
    sprintf(FilDistanceMM, " D: %d mm", FilDistance);

    if (ConfigMode) {
      //if in config state we will display the sensor values to the screen
     //and will set the ESP to AP mode so that user can configure device
      dnsServer.processNextRequest(); // used by captive portal
      Display_Sensors( FILSWpin,  Resetpin);
      
      delay(500);
    } else { 
      
      sprintf(Line1, "%s Watchdog",  (preferences.getString("PName")).substring(0, 9));

      //log values so we can see what is going on

      //char Telemetry[100] = "";
      //sprintf(Telemetry, "LastMeasuredPos: %i FilPos: %i FilamentHasMoved: %i  ErrorState: %i TimerComplete: %i ", LastMeasuredPos, FilPos, FilamentHasMoved, ErrorState, Timer_Complete);
      //Serial.println(Telemetry);

      //SendTelemetry ( LastMeasuredPos,  FilPos,  FilamentHasMoved,  ErrorState,  Timer_Complete,  FilDistanceMM );
     // if (ErrorState) {delay(10000);}


      if (!ErrorState) {
          //running in self mode
          char Line2[20] = "Filament Present";
          char Line3[20] = "Motion OK";
          //char Line4[20] = "Self Detect Mode";
          Display_Screen(Line1, Line2, Line3, FilDistanceMM);
          ErrorState = false;
          if(preferences.getBool("SelfMode")) {TriggerOutput(false);}
         
          if (!DetectFilament()) {
            char Line2[20] = "Filament Missing";
            char Line3[20] = "";
           // char Line4[20] = "Self Detect Mode";
            Display_Screen(Line1, Line2, Line3, FilDistanceMM);
             Serial.print("SelfMode: ");
                  Serial.println(preferences.getBool("SelfMode"));

            if(preferences.getBool("SelfMode")) {
              Serial.println("Triggering Output");
               TriggerOutput(true);}
            if(preferences.getBool("SendNote") && !ErrorState) {
              SendNotification (preferences.getString("FilRunout"));
            }
              ErrorState = true;
              
          } 
          LastMeasuredPos = ReadPosSensor();
          DetectMovement();
          if (Timer_Complete) {
            DisplayStopScreen();
            FilamentHasMoved = false;
            Timer_Complete = false;
          }


        //compatability mode means we just send a pulse every X mm (configurable)
        //as long as there are no errors
       
        if(!preferences.getBool("SelfMode") &&  FilDistance > 0) {
           /*if (abs(FilDistance% preferences.getInt("RunLen"))< 1){
               TriggerOutput(true);
                Serial.println("Pulse High");
           } 
           if (abs(FilDistance% (preferences.getInt("RunLen") + preferences.getInt("RunLen")/2)< 1)){
               TriggerOutput(false);
                Serial.println("Pulse Low");
           } 
         */
           if (abs(FilDistance % 2 ==0)){  //just do even odd based on mm runlenght
               TriggerOutput(true);
                Serial.println("Pulse High");
           } else {
                  TriggerOutput(false);
                  Serial.println("Pulse Low");
           }   

        }
      // delay (500);

      } else {
             
        //we are in error state - look for issues to be corrected and device can self reset
        LastMeasuredPos = ReadPosSensor();
         DetectMovement();
        if (DetectFilament() && FilamentHasMoved) {
          ResetSensor();
        }
      }
    }  
   
}
