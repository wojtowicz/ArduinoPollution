#include <ESP8266WiFi.h>
#include "WifiNetworks.h"
#include "WifiNetwork.h"

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
  struct WifiNetwork *ia = (struct WifiNetwork *)a;
  struct WifiNetwork *ib = (struct WifiNetwork *)b;
  return (int)(100.f*ib->signalQuality - 100.f*ia->signalQuality);
}

void WifiNetworks::buildWifiNetworks(struct WifiNetwork *wifiNetworks, int networksCount)
{
  for(int i = 0; i < networksCount; i++) {
    int32_t signalQuality = getSignalQuality(WiFi.RSSI(i));
    String encryptionType = getEncryptionType(WiFi.encryptionType(i));
    String ssid = WiFi.SSID(i);

    struct WifiNetwork wifiNetwork = { ssid, encryptionType, signalQuality };
    wifiNetworks[i] = wifiNetwork;
  }
}

void WifiNetworks::sortWifiNetworks(struct WifiNetwork *wifiNetworks, int networksCount)
{
  size_t structsLength = networksCount / sizeof(struct WifiNetwork);
  qsort(wifiNetworks, networksCount, sizeof(struct WifiNetwork), structCmpBySignalQuality);
}

String WifiNetworks::getSelectOptionsHtml(struct WifiNetwork *wifiNetworks, int networksCount){
  String result = "";
  for(int i = 0; i < networksCount; i++) {
    result += "<option value='" + wifiNetworks[i].ssid + "'>" + wifiNetworks[i].ssid + " (" + wifiNetworks[i].encryptionType + ", " + wifiNetworks[i].signalQuality + "%)</option>";
  }
  return result;
}

String WifiNetworks::renderSelectOptionsHtml() {
  int networksCount = WiFi.scanNetworks();
  struct WifiNetwork wifiNetworks[networksCount];
  Serial.println(networksCount);
  
  buildWifiNetworks(wifiNetworks, networksCount);
  sortWifiNetworks(wifiNetworks, networksCount);
  return getSelectOptionsHtml(wifiNetworks, networksCount);
}

String WifiNetworks::renderFormHtml() {
  String content =
    String("<p>Choose Wifi network</p>") +
          "<form action='.' method='POST'>" +
          "  <label for='ssid'><p>SSID</p></label>" +
          "  <select id='ssid' name='ssid'>" +
                  renderSelectOptionsHtml() +
          "  </select>" +
          "  <label for='password'><p>Password</p></label>" +
          "  <p><input id='password' type='password' name='password'/></p>" +
          "  <input type='submit' value='Save'/>" +
          "</form>";
  return content;
}
