/* This module contains all functions related to
 sending log messages
*/

#include <Preferences.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <UrlEncode.h>

extern Preferences preferences;
extern HTTPClient http;


void SendNotification (String NotificationMsg) {
   //connect to Wifi
   Serial.println(preferences.getString("SSID"));
   Serial.println(preferences.getString("PWD"));

    WiFi.begin(preferences.getString("SSID"), preferences.getString("PWD") );
    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      delay(500);
      Serial.println("");
    //test the push notification
    Serial.println("Sending Notification");
    String NotificationURL = preferences.getString("NoteURL") +
                             "phone=" + preferences.getString("NotePhone")  +
                             "&apikey=" + preferences.getString("NoteAPIKey") +
                             "&text=" + urlEncode(NotificationMsg);
    Serial.println(NotificationURL);                         
    http.begin(NotificationURL); //HTTP
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(NotificationURL);
    if (httpResponseCode == 200){
        Serial.print("Message sent successfully");
    }
    else{
        Serial.println("Error sending the message");
        Serial.print("HTTP response code: ");
        Serial.println(httpResponseCode);
        http.end();
    }
  }
}



    

    
