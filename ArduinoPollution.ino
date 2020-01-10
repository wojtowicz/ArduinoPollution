#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "TimerManager.h"
#include "WifiManager.h"
#include "LCDMessageStruct.h"
#include "MyLCD.h"
#include "PollutionService.h"
#include "WifiNetworks.h"

TimerManager timerManager = TimerManager::getInstance();
WifiManager wifiManager = WifiManager::getInstance();
MyLCD myLCD;
PollutionService pollutionService;
ESP8266WebServer server(80);
WifiNetworks wifiNetworks;

void handleRoot() {
  wifiNetworks.setCurrentSSID(wifiManager.getSSID());
  server.send(200, "text/html", wifiNetworks.renderFormHtml());
}

void handleForm() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Headers", "origin, content-type, accept");
  String message = "<p>Connecting with WiFi...</p><p>Please look at LCD instructions</p><p><a href='/'>Back</a><p>";
  if (server.method() == HTTP_POST) {
    for (uint8_t i = 0; i < server.args(); i++) {
      if(server.argName(i) == "ssid"){
        wifiManager.setSSID(server.arg(i));
      }
      if(server.argName(i) == "password"){
        wifiManager.setPassword(server.arg(i));
      }
      Serial.println("server.argName(i)");
      Serial.println(server.argName(i));
      if(server.argName(i) == "plain"){
        message = "";
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, server.arg("plain"));

        if (!error && !doc.isNull()) {
          String ssid = doc["ssid"];
          String password = doc["password"];
          Serial.println(ssid);
          Serial.println(password);
          wifiManager.setSSID(ssid);
          wifiManager.setPassword(password);
        }
      }
    }
    server.send(200, "text/html", message);
    wifiManager.setConnectWifiToOn();
  } else if (server.method() == HTTP_OPTIONS){
    server.send(200, "text/plain", "");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleDisconnectForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    String message = "<p>Disconnected<a href='/'>Back</a><p>";
    server.send(200, "text/html", message);
    wifiManager.setDisconnectWifiToOn();
  }
}

void setup() {
  delay(1000);
  Serial.begin(9600);

  pollutionService.setup();
  myLCD.configure();
  wifiManager.setup();
  
  server.on("/", handleRoot);
  server.on("/wifi_networks/connect", handleForm);
  server.on("/wifi_networks/disconnect", handleDisconnectForm);
  server.begin();
  
  Serial.println("HTTP server started");
}

void loop() {
  timerManager.setTimer(millis());
  server.handleClient();
  
  if(wifiManager.isConnectWifi()){
    delay(1000);
    wifiManager.connect();
  }

  if(wifiManager.isDisconnectWifi()){
    delay(1000);
    wifiManager.disconnect();
    pollutionService.ledsOff();
  }

  if (timerManager.isTimerDefaultExceeded()) {
    Serial.println("WiFi.status");
    Serial.println(WiFi.status());
  }

  if(wifiManager.isWifiConnecting()){
    wifiManager.handleWifiConnection();
  }

  if(WiFi.status() == WL_CONNECTED){
    if (timerManager.isTimerWebFetcherExceeded()) {
      pollutionService.fetch();
    }
  }
  
  if (timerManager.isTimerDefaultExceeded()) {
    timerManager.setTimerDefault();
  }

  if (timerManager.isTimerWebFetcherExceeded()) {
    timerManager.setTimerWebFetcher();
  }
}
