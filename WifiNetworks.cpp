#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "WifiNetworks.h"
#include "WifiNetworkStruct.h"

void WifiNetworks::setCurrentSSID(String ssid) {
  this->currentSSID = ssid;
}

bool WifiNetworks::hasCurrentSSID() {
  return this->currentSSID != "";
}

int32_t WifiNetworks::getSignalQuality(int32_t signalStrength) {
  int32_t signalQuality;

  if(signalStrength <= -100) {
    signalQuality = 0;
  }
  else if(signalStrength >= -50) {
    signalQuality = 100;
  }
  else {
    signalQuality = 2 * (signalStrength + 100);
  }

  return signalQuality;
}

String WifiNetworks::getEncryptionType(uint8_t wifiEcryptionType) {
  String encryptionType;
  switch(wifiEcryptionType) {
    case ENC_TYPE_WEP:
      encryptionType = "WEP";
      break;
    case ENC_TYPE_TKIP:
      encryptionType = "WPA/PSK";
      break;
    case ENC_TYPE_CCMP:
      encryptionType = "WPA2/PSK";
      break;
    case ENC_TYPE_AUTO:
      encryptionType = "WPA/WPA2/PSK";
      break;
    case ENC_TYPE_NONE:
      encryptionType = "Open";
      break;
    default:
      encryptionType = "?";
  }
  return encryptionType;
}

int WifiNetworks::structCmpBySignalQuality(const void *a, const void *b)
{
  struct WifiNetworkStruct *ia = (struct WifiNetworkStruct *)a;
  struct WifiNetworkStruct *ib = (struct WifiNetworkStruct *)b;
  return (int)(100.f*ib->signalQuality - 100.f*ia->signalQuality);
}

void WifiNetworks::buildWifiNetworks(struct WifiNetworkStruct *wifiNetworks, int networksCount)
{
  for(int i = 0; i < networksCount; i++) {
    int32_t signalQuality = getSignalQuality(WiFi.RSSI(i));
    String encryptionType = getEncryptionType(WiFi.encryptionType(i));
    String ssid = WiFi.SSID(i);

    struct WifiNetworkStruct wifiNetwork = { ssid, encryptionType, signalQuality };
    wifiNetworks[i] = wifiNetwork;
  }
}

void WifiNetworks::sortWifiNetworks(struct WifiNetworkStruct *wifiNetworks, int networksCount)
{
  size_t structsLength = networksCount / sizeof(struct WifiNetworkStruct);
  qsort(wifiNetworks, networksCount, sizeof(struct WifiNetworkStruct), structCmpBySignalQuality);
}

String WifiNetworks::getSelectOptionsHtml(struct WifiNetworkStruct *wifiNetworks, int networksCount){
  String result = "";
  for(int i = 0; i < networksCount; i++) {
    result += "<option value='" + wifiNetworks[i].ssid + "'>" + wifiNetworks[i].ssid + " (" + wifiNetworks[i].encryptionType + ", " + wifiNetworks[i].signalQuality + "%)</option>";
  }
  return result;
}

String WifiNetworks::getSelectOptionsJson(struct WifiNetworkStruct *wifiNetworks, int networksCount){
  DynamicJsonDocument doc(2048);
  String result = "";

  for(int i = 0; i < networksCount; i++) {
    JsonObject obj = doc.createNestedObject();
    obj["ssid"] = wifiNetworks[i].ssid;
    obj["encryption_type"] = wifiNetworks[i].encryptionType;
    obj["signal_quality"] = wifiNetworks[i].signalQuality;
  }
  result = doc.as<String>();
  return result;
}

String WifiNetworks::renderSelectOptions(String format) {
  int networksCount = WiFi.scanNetworks();
  struct WifiNetworkStruct wifiNetworks[networksCount];
  Serial.println(networksCount);
  
  buildWifiNetworks(wifiNetworks, networksCount);
  sortWifiNetworks(wifiNetworks, networksCount);
  String selectOptions = "";
  if(format == "html"){
    selectOptions = getSelectOptionsHtml(wifiNetworks, networksCount);
  }
  if (format == "json"){
    selectOptions = getSelectOptionsJson(wifiNetworks, networksCount);
  }
  return selectOptions;
}

String WifiNetworks::renderCurrentSSIDHtml() {
  if(!hasCurrentSSID()) return String("");
  String content =
    String("<form action='/wifi_networks/disconnect' method='POST'>") +
    "<p>Current SSID: " + 
    this->currentSSID + 
    "<input type='submit' value='Disconnect'/>" +
    "</p>" +
    "</form>";
  return content;
}

String WifiNetworks::renderFormHtml() {
  String content =
    String(renderCurrentSSIDHtml()) +
          "<p>Choose Wifi network</p>" +
          "<form action='/wifi_networks/connect' method='POST'>" +
          "  <label for='ssid'><p>SSID</p></label>" +
          "  <select id='ssid' name='ssid'>" +
                  renderSelectOptions("html") +
          "  </select>" +
          "  <label for='password'><p>Password</p></label>" +
          "  <p><input id='password' type='password' name='password'/></p>" +
          "  <input type='submit' value='Save'/>" +
          "</form>";
  return content;
}
