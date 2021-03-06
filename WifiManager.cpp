#include <ESP8266WiFi.h>
#include "WifiManager.h"

#include "ConfigManager.h"
#include "MyLCD.h"
#include "LCDMessageStruct.h"

extern MyLCD myLCD;

#ifndef APSSID
#define APSSID "SmartHome"
#endif

const char *apssid = APSSID;

ConfigManager configManager = ConfigManager::getInstance();

WifiManager::WifiManager() { }

WifiManager& WifiManager::getInstance() {
  static WifiManager instance;
  return instance;
}

void WifiManager::setup(){
  configManager.load();
  this->ssid = configManager.getSSID();
  this->password = configManager.getPassword();
  this->connectWifi = false;
  this->disconnectWifi = false;
  this->wifiConnecting = false;

  if(configManager.isDeviceConfigured()){
    setConnectWifiToOn();
  } else {
    if(WiFi.status() == WL_CONNECTED) {
      displayWifiInfo();
    } else {
      startWifiAP();
    }
  }
}

void WifiManager::connect(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(getSSID(), getPassword());
  setConnectWifiToOff();
  setWifiConnectingToOn();
}

void WifiManager::disconnect(){
  setSSID("");
  setPassword("");
  saveConfig();
  setDisconnectWifiToOff();
  startWifiAP();
}

void WifiManager::saveConfig(){
  configManager.setSSID(getSSID());
  configManager.setPassword(getPassword());
  configManager.save();
}

void WifiManager::clear() {
  this->connectWifi = false;
  this->disconnectWifi = false;
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

boolean WifiManager::isDisconnectWifi() {
  return this->disconnectWifi;
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

void WifiManager::setDisconnectWifiToOn() {
  this->disconnectWifi = true;
}

void WifiManager::setDisconnectWifiToOff() {
  this->disconnectWifi = false;
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
      saveConfig();
      displayWifiInfo();
      displayMessageOnLCD("", 1);
      break;
  }
}
