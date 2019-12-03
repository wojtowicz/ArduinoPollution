#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "WifiNetworks.h"

#ifndef APSSID
#define APSSID "SmartHome"
#endif

const char *ssid = APSSID;

LiquidCrystal_PCF8574 lcd(0x27);
ESP8266WebServer server(80);

WifiNetworks wifiNetworks;

void handleRoot() {
  server.send(200, "text/html", wifiNetworks.renderFormHtml());
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
  
  Serial.print("Configuring access point...");
  lcd.print("Configuring...");
  
  WiFi.softAP(ssid);
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  lcd.home();
  lcd.clear();
  lcd.print(WiFi.softAPIP());
  
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
