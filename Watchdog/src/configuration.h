/* This module contains all functions related to
configuration of the watchdog sensor
*/
#include "SPIFFS.h"
#include <Preferences.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <WiFi.h>


extern Preferences preferences;
extern AsyncWebServer server;
extern const char* ssid;
extern const char* password;
extern const byte DNS_PORT;

void ProcessGet(AsyncWebServerRequest *request);
void notFound(AsyncWebServerRequest *request);
String processor(const String& var);

DNSServer dnsServer; 

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
    CaptiveRequestHandler() {

    // Route to load index.html file
    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });

      // Route to load style.css file
    server.on("/main.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/main.css", "text/css");
    });

    // Route to load javascript file
    server.on("/app.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/app.js", "text/css");
    });
    
    // Route to process get 
      server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request){
      ProcessGet(request) ;
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });
    server.onNotFound(notFound);

    }
    virtual ~CaptiveRequestHandler() {}
    bool canHandle(AsyncWebServerRequest *request)
    {
        return true;
    }
    void handleRequest(AsyncWebServerRequest *request)
    { 
     
     //request->send(200, "text/html", "<a href=\"http://index.html\">Configuration Page</a>"); 
      request->send(SPIFFS, "/index.html", String(), false, processor);
    
    }
};


void SetupWiFi (){

   
    WiFi.disconnect();   //added to start with the wifi off, avoid crashing
    WiFi.mode(WIFI_OFF); //added to start with the wifi off, avoid crashing
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP("Watchdog Configuration");
    // if DNSServer is started with "*" for domain name, it will reply with
    // provided IP to all DNS request
    dnsServer.start(DNS_PORT, "*", apIP);
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  
    server.begin();
 
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void ProcessGet(AsyncWebServerRequest *request) {

 String inputMessage;
    if(request->hasParam("PName")) {
      inputMessage = request->getParam("PName")->value();
      preferences.putString("PName", inputMessage);
     }
     if(request->hasParam("ckbInvertOut")) {
      inputMessage = request->getParam("ckbInvertOut")->value();
      Serial.println(inputMessage);
      if (inputMessage =="on") {
         preferences.putBool("InvertOut", true); 
      } else {
        preferences.putBool("InvertOut", false);  
      }
     } else {
       preferences.putBool("InvertOut", false);
     }
     if(request->hasParam("ckbInvertScrn")) {
      inputMessage = request->getParam("ckbInvertScrn")->value();
      if (inputMessage =="on") {
         preferences.putBool("InvertScn", true); 
      } else {
        preferences.putBool("InvertScn", false);  
      }
     }else {
       preferences.putBool("InvertScn", false);
     }
    
    

     if(request->hasParam("ckbMonFil")) {
      inputMessage = request->getParam("ckbMonFil")->value();
      if (inputMessage =="on") {
         preferences.putBool("MonFil", true); 
      } else {
        preferences.putBool("MonFil", false);  
      }
     }else {
       preferences.putBool("MonFil", false);
     }

     if(request->hasParam("Mode")) {
      inputMessage = request->getParam("Mode")->value();
      if (inputMessage =="Self") {
         preferences.putBool("SelfMode", true);
      } else {
        preferences.putBool("SelfMode", false);  
      }
     }

     if(request->hasParam("Behavior")) {
      inputMessage = request->getParam("Behavior")->value();
      if (inputMessage =="NO") {
         preferences.putBool("NotifyOnly", true);
      } else {
        preferences.putBool("NotifyOnly", false);  
      }
     }

   
     if(request->hasParam("DelaySec")) {
      inputMessage = request->getParam("DelaySec")->value();
      preferences.putInt("DelaySec", inputMessage.toInt());
     }

      if(request->hasParam("RotationMM")) {
      inputMessage = request->getParam("RotationMM")->value();
      preferences.putFloat("RotationMM", inputMessage.toFloat());
     }

      if(request->hasParam("Sensitivity")) {
      inputMessage = request->getParam("Sensitivity")->value();
      preferences.putInt("Sensitivity", inputMessage.toInt());
     }
      if(request->hasParam("Threshold")) {
      inputMessage = request->getParam("Threshold")->value();
      preferences.putInt("Threshold", inputMessage.toInt());
     }

     if(request->hasParam("RunLen")) {
      inputMessage = request->getParam("RunLen")->value();
      preferences.putInt("RunLen", inputMessage.toInt());
     }
     if(request->hasParam("PulseWidth")) {
      inputMessage = request->getParam("PulseWidth")->value();
      preferences.putInt("PulseWidth", inputMessage.toInt());
     }

     if(request->hasParam("ckbSendNote")) {
      inputMessage = request->getParam("ckbSendNote")->value();
      if (inputMessage =="on") {
         preferences.putBool("SendNote", true); 
      } else {
        preferences.putBool("SendNote", false);  
      }
     } else {
        preferences.putBool("SendNote", false);  
       
     }
     
     if(request->hasParam("NoteURL")) {
      inputMessage = request->getParam("NoteURL")->value();
      preferences.putString("NoteURL", inputMessage);
     }

     if(request->hasParam("NotePhone")) {
      inputMessage = request->getParam("NotePhone")->value();
      preferences.putString("NotePhone", inputMessage);
     }
     if(request->hasParam("NoteAPIKey")) {
      inputMessage = request->getParam("NoteAPIKey")->value();
      preferences.putString("NoteAPIKey", inputMessage);
     }
     if(request->hasParam("FilRunout")) {
      inputMessage = request->getParam("FilRunout")->value();
      preferences.putString("FilRunout", inputMessage);
     }
     if(request->hasParam("FilStop")) {
      inputMessage = request->getParam("FilStop")->value();
      preferences.putString("FilStop", inputMessage);
     }

     if(request->hasParam("SSID")) {
      inputMessage = request->getParam("SSID")->value();
      preferences.putString("SSID", inputMessage);
     }
     if(request->hasParam("PWD")) {
      inputMessage = request->getParam("PWD")->value();
      preferences.putString("PWD", inputMessage);
     } 
    
    
}


String processor(const String& var)
{
 
  //Serial.println(var);
 
  if(var == "PNAME") {
    return  preferences.getString("PName");
  } else if (var == "InvertOut") {
    if (preferences.getBool("InvertOut")){
        Serial.println("returning checked");
        return "checked"; 
   }
  } else if (var == "InvertScn") {
    if (preferences.getBool("InvertScn")){
        return "checked"; 
    } 
  } else if (var == "MonFil") {
    if (preferences.getBool("MonFil")){
        return "checked"; 
    } 
  } else if (var == "ifMon") {
    if (preferences.getBool("MonFil")){
        return ""; 
    } else return "none";

  } else if (var == "SelfMode") {
    if (preferences.getBool("SelfMode")){
        return "checked"; 
    } 
  } else if (var == "HostMode") {
    if (!preferences.getBool("SelfMode")){
        return "checked"; 
    } 

    } else if (var == "NoteBehavior") {
    if (preferences.getBool("NotifyOnly")){
        return "checked"; 
    } 
  } else if (var == "ErrorBehavior") {
    if (!preferences.getBool("NotifyOnly")){
        return "checked"; 
    } 

  } else if (var == "ifHost") {
    if (!preferences.getBool("SelfMode")){
        return ""; 
    } else return "none";
  } else if(var == "DelaySec") {
    return  String(preferences.getInt("DelaySec"));
  
  } else if(var == "RotationMM") {
    return  String(preferences.getFloat("RotationMM"));

  } else if(var == "Sensitivity") {
    return  String(preferences.getInt("Sensitivity"));
  } else if(var == "Threshold") {
    return  String(preferences.getInt("Threshold"));

  } else if(var == "RunLen") {
    return  String(preferences.getInt("RunLen"));
  } else if(var == "PulseWidth") {
    return  String(preferences.getInt("PulseWidth"));
  }  else if (var == "SendNote") {
    if (preferences.getBool("SendNote")){
        return "checked"; 
    } 
  } else if (var == "ifSendNote") {
    if (!preferences.getBool("SendNote")){
        return "none"; 
    } else return ""; 

  } else if(var == "NoteURL") {
    return  preferences.getString("NoteURL");
  } else if(var == "NotePhone") {
    return  preferences.getString("NotePhone");
  } else if(var == "NoteAPIKey") {
    return  preferences.getString("NoteAPIKey");
  } else if(var == "FilRunout") {
    return  preferences.getString("FilRunout");
  } else if(var == "FilStop") {
    return  preferences.getString("FilStop");
  } else if(var == "SSID") {
    return  preferences.getString("SSID");
  } else if(var == "PWD") {
    return  preferences.getString("PWD");
  } 
   return String();
}



