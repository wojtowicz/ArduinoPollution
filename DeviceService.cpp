#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#include "DeviceService.h"

extern WiFiClient client;
extern HTTPClient http;

void DeviceService::sendLocalIp(String uuid, IPAddress localIp) {
  Serial.println("[HTTP] begin...");
  String url = "http://tomash-arduino-api.fly.dev/devices/" + uuid + ".json";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  Serial.println("[HTTP] PUT...");
  int httpCode = http.PUT("{\"local_ip\":\"" + localIp.toString() + "\"}");

  if (httpCode > 0) {
    Serial.printf("[HTTP] PUT... code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
      const String& payload = http.getString();
      Serial.println("received payload:\n<<");
      Serial.println(payload);
      Serial.println(">>");
    }
  } else {
    Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
    http.end();
}
