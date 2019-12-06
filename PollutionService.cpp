#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include "LCDMessageStruct.h"
#include "MyLCD.h"
#include "PollutionService.h"
#include "PollutionStruct.h"

extern MyLCD myLCD;
PollutionStruct pollution;

#define LED_RED D5
#define LED_YELLOW D6
#define LED_GREEN D7

WiFiClient client;
HTTPClient http;

void PollutionService::setup(){
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void PollutionService::fetch() {
  Serial.println("[HTTP] begin...");
  if (http.begin(client, "http://tomash-arduino-api.herokuapp.com/pollution/measurements?lat=50.00983&lng=19.97484&fields[]=airly_caqi&fields[]=pm25&fields[]=pm10")) {
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {

        parse();
        displayOnLCD();
        blinkLeds();
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  } else {
    Serial.printf("[HTTP} Unable to connect\n");
  }
}

void PollutionService::parse(){
  String payload = http.getString();
  int index;
  index = payload.indexOf("DATA:");
  String newString = payload.substring(index + 5);
  
  index = newString.indexOf("|");
  pollution.level = newString.substring(0, index);
  Serial.println("Level:" + pollution.level);
  newString = newString.substring(pollution.level.length() + 1);

  index = newString.indexOf("|");
  pollution.pm25 = newString.substring(0, index);
  Serial.println("PM25:" + pollution.pm25);
  newString = newString.substring(pollution.pm25.length() + 1);

  index = newString.indexOf("|");
  pollution.pm10 = newString.substring(0, index);
  Serial.println("PM10:" + pollution.pm10);
  newString = newString.substring(pollution.pm10.length() + 1);
  Serial.println(payload);
}

void PollutionService::displayOnLCD(){
  myLCD.clear();
  myLCD.display({ "PM2.5: " + pollution.pm25 + "%", 0, 0, false, ""});
  myLCD.display({ "PM10: " + pollution.pm10 + "%", 0, 1, false, ""});
}

void PollutionService::blinkLeds(){
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);
  
  if(pollution.level == "1"){
    Serial.println("Level 1");
    digitalWrite(LED_GREEN, HIGH);
  }
  if(pollution.level == "2"){
    Serial.println("Level 2");
    digitalWrite(LED_YELLOW, HIGH);
  }
  if(pollution.level == "3"){
    Serial.println("Level 3");
    digitalWrite(LED_RED, HIGH);
  }
}
