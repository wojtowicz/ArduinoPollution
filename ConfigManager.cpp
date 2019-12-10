#include <ESP8266WiFi.h>
#include "ConfigManager.h"
#include <ArduinoJson.h>
#include "FS.h"

ConfigManager::ConfigManager() {
  SPIFFS.begin();
  this->ssid = "";
  this->pass = "";
}

ConfigManager& ConfigManager::getInstance() {
  static ConfigManager instance;
  return instance;
}

bool ConfigManager::load() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    return false;
  }
  
  size_t size = configFile.size();

  char *buf = new char[size];
  
  configFile.readBytes(buf, size);

  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, buf);
  JsonObject obj = doc.as<JsonObject>();
  
  if (error || doc.isNull()) {
    delete buf;
    return false;
  }

  this->ssid = obj["ssid"].as<String>();
  this->pass = obj["pass"].as<String>();
  
  delete buf;
  configFile.close();
  return true;
}

bool ConfigManager::save() {
  DynamicJsonDocument doc(200);
  doc["ssid"] = this->ssid;
  doc["pass"] = this->pass;
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    return false;
  }
  serializeJson(doc, configFile);
  configFile.close();
  return true;
  
}

bool ConfigManager::reset() {
  setSSID("");
  setPassword("");
  return save();
}

bool ConfigManager::isDeviceConfigured() {
  return this->ssid != "";
}

String ConfigManager::getSSID() {
  return this->ssid;
}

void ConfigManager::setSSID(String ssid) {
  this->ssid = ssid;
}

String ConfigManager::getPassword() {
  return this->pass;
}

void ConfigManager::setPassword(String password) {
  this->pass = password;
}
