#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include "LCDMessage.h"
#include "MyLCD.h"
#include "WifiNetworks.h"

#ifndef APSSID
#define APSSID "SmartHome"
#endif

#define LED_RED D5
#define LED_YELLOW D6
#define LED_GREEN D7

const char *apssid = APSSID;
const String arduinoApiHost = "https://tomash-arduino-api.herokuapp.com";

MyLCD myLCD;
ESP8266WebServer server(80);
WifiNetworks wifiNetworks;

String ssid;
String password;
boolean connectWifi = false;
boolean wifiConnecting = false;
static uint32_t timer;
static uint32_t timer2;
int a = 1;

void handleRoot() {
  server.send(200, "text/html", wifiNetworks.renderFormHtml());
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    for (uint8_t i = 0; i < server.args(); i++) {
      if(server.argName(i) == "ssid"){
        ssid = server.arg(i);
      }
      if(server.argName(i) == "password"){
        password = server.arg(i);
      }
    }
    String message = "<p>Connecting with WiFi...</p><p>Please look at LCD instructions</p><p><a href='/'>Back</a><p>";
    server.send(200, "text/html", message);
    connectWifi = true;
  }
}

void startWifiAP() {
  connectWifi = false;
  wifiConnecting = false;
  WiFi.disconnect();

  myLCD.clear();
  myLCD.display({ "Configuring", 0, 0, true, "Configuring access point..."});
  
  WiFi.softAP(apssid);
  
  IPAddress softAPIP = WiFi.softAPIP();
  myLCD.clear();
  myLCD.display({ softAPIP.toString(), 0, 0, true, "AP IP: "});
}

void displayMessageOnLCD(String message, int row){
  myLCD.display({ message, 0, row, false, ""});
}

void displayWifiInfo() {
  wifiConnecting = false;
  IPAddress localIp = WiFi.localIP();
  myLCD.clear();
  myLCD.display({ localIp.toString(), 0, 0, true, "IP address: "});
}

void handleWifiConnection(){
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      displayMessageOnLCD("idle", 1);
    case WL_DISCONNECTED:
      displayMessageOnLCD("Wifi connecting.", 1);
      if (millis() > timer) {
        timer = millis() + 5000;
        Serial.println("[loop] no wifi");
      }
      break;
    case WL_NO_SSID_AVAIL:
      startWifiAP();
      displayMessageOnLCD("Wrong ssid", 1);
      break;
    case WL_CONNECT_FAILED:
      startWifiAP();
      displayMessageOnLCD("Wrong password", 1);
      break;
    case WL_CONNECTED:
      displayWifiInfo();
      displayMessageOnLCD("", 1);
      delay(500);
      break;
  }
}

void setup() {
  delay(1000);
  Serial.begin(9600);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_GREEN, LOW);

  myLCD.configure();

  if(WiFi.status() == WL_CONNECTED) {
    displayWifiInfo();
  } else {
    startWifiAP();
  }
  
  server.on("/", handleRoot);
  server.on("/wifi_networks/", handleForm);
  server.begin();
  
  
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  if(connectWifi){
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    connectWifi = false;
    wifiConnecting = true;
  }

  if (millis() > timer) {
    timer = millis() + 5000;
    Serial.println("WiFi.status");
    Serial.println(WiFi.status());
  }

  if(wifiConnecting){
    handleWifiConnection();
  }

  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;

    if (millis() > timer) {
      timer = millis() + 5000;
      Serial.println("[HTTP] begin...");
      if (http.begin(client, "http://tomash-arduino-api.herokuapp.com/pollution/measurements?lat=50.00983&lng=19.97484&fields[]=airly_caqi&fields[]=pm25&fields[]=pm10")) {
        Serial.print("[HTTP] GET...\n");
        int httpCode = http.GET();
        if (httpCode > 0) {
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = http.getString();
            int index;
            index = payload.indexOf("DATA:");
            String newString = payload.substring(index + 5);
            
            index = newString.indexOf("|");
            String level = newString.substring(0, index);
            Serial.println("Level:" + level);
            newString = newString.substring(level.length() + 1);
          
            index = newString.indexOf("|");
            String pm25 = newString.substring(0, index);
            Serial.println("PM25:" + pm25);
            newString = newString.substring(pm25.length() + 1);
          
            index = newString.indexOf("|");
            String pm10 = newString.substring(0, index);
            Serial.println("PM10:" + pm10);
            newString = newString.substring(pm10.length() + 1);
            Serial.println(payload);

            myLCD.clear();
            myLCD.display({ "PM2.5: " + pm25 + "%", 0, 0, false, ""});
            myLCD.display({ "PM10: " + pm10 + "%", 0, 1, false, ""});

            digitalWrite(LED_RED, LOW);
            digitalWrite(LED_YELLOW, LOW);
            digitalWrite(LED_GREEN, LOW);
            
            if(level == "1"){
              Serial.println("Level 1");
              digitalWrite(LED_GREEN, HIGH);
            }
            if(level == "2"){
              Serial.println("Level 2");
              digitalWrite(LED_YELLOW, HIGH);
            }
            if(level == "3"){
              Serial.println("Level 3");
              digitalWrite(LED_RED, HIGH);
            }
          }
        } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
  
        http.end();
      } else {
        Serial.printf("[HTTP} Unable to connect\n");
      }
    }
  }
}
