#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#include "LCDMessageStruct.h"
#include "MyLCD.h"
#include "PollutionService.h"
#include "WifiNetworks.h"

#ifndef APSSID
#define APSSID "SmartHome"
#endif

const char *apssid = APSSID;
const String arduinoApiHost = "https://tomash-arduino-api.herokuapp.com";

MyLCD myLCD;
PollutionService pollutionService;
ESP8266WebServer server(80);
WifiNetworks wifiNetworks;

String ssid;
String password;
boolean connectWifi = false;
boolean wifiConnecting = false;
static uint32_t timer;
static uint32_t timerDefault;
static uint32_t timerWebFetcher;

boolean timerDefaultExceeded(){
  return timer - timerDefault >= 5000;
}

boolean timerWebFetcherExceeded(){
  return timer - timerWebFetcher >= 30000;
}

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
      if (timerDefaultExceeded()) {
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

  pollutionService.setup();

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
  timer = millis();
  server.handleClient();
  
  if(connectWifi){
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    connectWifi = false;
    wifiConnecting = true;
  }

  if (timerDefaultExceeded()) {
    Serial.println("WiFi.status");
    Serial.println(WiFi.status());
  }

  if(wifiConnecting){
    handleWifiConnection();
  }

  if(WiFi.status() == WL_CONNECTED){
    if (timerWebFetcherExceeded()) {
      pollutionService.fetch();
    }
  }
  
  if (timerDefaultExceeded()) {
    timerDefault = timer;
  }

  if (timerWebFetcherExceeded()) {
    timerWebFetcher = timer;
  }
}
