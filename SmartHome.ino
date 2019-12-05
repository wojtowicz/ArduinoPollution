#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "WifiNetworks.h"

#ifndef APSSID
#define APSSID "SmartHome"
#endif

const char *apssid = APSSID;

LiquidCrystal_PCF8574 lcd(0x27);
ESP8266WebServer server(80);
WifiNetworks wifiNetworks;

String ssid;
String password;
boolean connectWifi = false;
boolean wifiConnecting = false;
static uint32_t timer;

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
  
  Serial.print("Configuring access point...");
  lcd.print("Configuring...");
  
  WiFi.softAP(apssid);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  lcd.home();
  lcd.clear();
  lcd.print(WiFi.softAPIP());
}

void displayMessageOnLCD(String message, int line){
  lcd.setCursor(0, line);
  lcd.print(message);
}

void displayWifiInfo() {
  wifiConnecting = false;
  Serial.println("[loop] WiFi connected");
  Serial.print("[loop] IP address: ");
  Serial.println(WiFi.localIP());
  lcd.home();
  lcd.clear();
  lcd.print(WiFi.localIP());
}

void handleWifiConnection(){
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      lcd.setCursor(0, 1);
      lcd.print("idle");
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

  int error;
  
  Serial.println("Configuring LCD...");
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");
    lcd.begin(16, 2);
    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
  } else {
    Serial.println(": LCD not found.");
  }

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
}
