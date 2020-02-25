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
#include "DeviceService.h"

TimerManager timerManager = TimerManager::getInstance();
WifiManager wifiManager = WifiManager::getInstance();
MyLCD myLCD;
PollutionService pollutionService;
ESP8266WebServer server(80);
WifiNetworks wifiNetworks;
DeviceService deviceService;

String uuid;
boolean syncLocalIp = true;

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
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Headers", "origin, content-type, accept");
  if (server.method() == HTTP_POST) {
    String message = "<p>Disconnected<a href='/'>Back</a><p>";
    server.send(200, "text/html", message);
    wifiManager.setDisconnectWifiToOn();
  } else if (server.method() == HTTP_OPTIONS){
    server.send(200, "text/plain", "");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleWifiNetworks() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Headers", "origin, content-type, accept");
  server.send(200, "application/json", wifiNetworks.renderSelectOptions("json"));
}

void handleInfo() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Headers", "origin, content-type, accept");
  DynamicJsonDocument doc(2048);
  String macAddress = WiFi.macAddress();
  doc["mac_address"] = macAddress;
  doc["chip_id"] = uuid;
  String infoJson = doc.as<String>();
  
  server.send(200, "application/json", infoJson);
}

void setup() {
  delay(1000);
  Serial.begin(9600);

  uuid = system_get_chip_id();

  pollutionService.setup();
  myLCD.configure();
  wifiManager.setup();
  
  server.on("/", handleRoot);
  server.on("/wifi_networks/connect", handleForm);
  server.on("/wifi_networks/disconnect", handleDisconnectForm);
  server.on("/wifi_networks", handleWifiNetworks);
  server.on("/info", handleInfo);
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
    syncLocalIp = true;
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
      pollutionService.fetch(uuid);
    }
    if (syncLocalIp) {
      deviceService.sendLocalIp(uuid, WiFi.localIP());
      syncLocalIp = true;
    }
  }
  
  if (timerManager.isTimerDefaultExceeded()) {
    timerManager.setTimerDefault();
  }

  if (timerManager.isTimerWebFetcherExceeded()) {
    timerManager.setTimerWebFetcher();
  }
}
