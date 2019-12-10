#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

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
  server.send(200, "text/html", wifiNetworks.renderFormHtml());
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
  } else {
    for (uint8_t i = 0; i < server.args(); i++) {
      if(server.argName(i) == "ssid"){
        wifiManager.setSSID(server.arg(i));
      }
      if(server.argName(i) == "password"){
        wifiManager.setPassword(server.arg(i));
      }
    }
    String message = "<p>Connecting with WiFi...</p><p>Please look at LCD instructions</p><p><a href='/'>Back</a><p>";
    server.send(200, "text/html", message);
    wifiManager.setConnectWifiToOn();
  }
}

void setup() {
  delay(1000);
  Serial.begin(9600);

  pollutionService.setup();
  myLCD.configure();
  wifiManager.setup();
  
  server.on("/", handleRoot);
  server.on("/wifi_networks/", handleForm);
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
