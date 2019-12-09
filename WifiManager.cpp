#include <ESP8266WiFi.h>
#include "WifiManager.h"

#include "MyLCD.h"
#include "LCDMessageStruct.h"

extern MyLCD myLCD;

#ifndef APSSID
#define APSSID "SmartHome"
#endif

const char *apssid = APSSID;

WifiManager::WifiManager() {
  this->ssid = "";
  this->password = "";
  this->connectWifi = false;
  this->wifiConnecting = false;
}

WifiManager& WifiManager::getInstance() {
  static WifiManager instance;
  return instance;
}

void WifiManager::setup(){
  if(WiFi.status() == WL_CONNECTED) {
    displayWifiInfo();
  } else {
    startWifiAP();
  }
}

void WifiManager::connect(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(getSSID(), getPassword());
  setConnectWifiToOff();
  setWifiConnectingToOn();
}

void WifiManager::clear() {
  this->connectWifi = false;
  this->wifiConnecting = false;
  WiFi.disconnect();
}

String WifiManager::getSSID() {
  return this->ssid;
}

String WifiManager::getPassword() {
  return this->password;
}

void WifiManager::setSSID(String ssid) {
  this->ssid = ssid;
}

void WifiManager::setPassword(String password) {
  this->password = password;
}
boolean WifiManager::isConnectWifi() {
  return this->connectWifi;
}

boolean WifiManager::isWifiConnecting() {
  return this->wifiConnecting;
}

void WifiManager::setConnectWifiToOn() {
  this->connectWifi = true;
}

void WifiManager::setConnectWifiToOff() {
  this->connectWifi = false;
}

void WifiManager::setWifiConnectingToOn() {
  this->wifiConnecting = true;
}

void WifiManager::setWifiConnectingToOff() {
  this->wifiConnecting = false;
}

void WifiManager::displayMessageOnLCD(String message, int row){
  myLCD.display({ message, 0, row, false, ""});
}

void WifiManager::displayWifiInfo() {
  setWifiConnectingToOff();
  IPAddress localIp = WiFi.localIP();
  myLCD.clear();
  myLCD.display({ localIp.toString(), 0, 0, true, "IP address: "});
}

void WifiManager::startWifiAP() {
  clear();

  myLCD.clear();
  myLCD.display({ "Configuring", 0, 0, true, "Configuring access point..."});
  
  WiFi.softAP(apssid);
  
  IPAddress softAPIP = WiFi.softAPIP();
  myLCD.clear();
  myLCD.display({ softAPIP.toString(), 0, 0, true, "AP IP: "});
}

void WifiManager::handleWifiConnection(){
  switch(WiFi.status()) {
    case WL_IDLE_STATUS:
      displayMessageOnLCD("idle", 1);
    case WL_DISCONNECTED:
      displayMessageOnLCD("Wifi connecting.", 1);
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
      break;
  }
}
