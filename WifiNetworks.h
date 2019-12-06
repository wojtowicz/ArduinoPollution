#include <ESP8266WiFi.h>

class WifiNetworks {
    private:
      int32_t getSignalQuality(int32_t signalStrength);
      String getEncryptionType(uint8_t wifiEcryptionType);
      void buildWifiNetworks(struct WifiNetworkStruct *wifiNetworks, int networksCount);
      void sortWifiNetworks(struct WifiNetworkStruct *wifiNetworks, int networksCount);
      String getSelectOptionsHtml(struct WifiNetworkStruct *wifiNetworks, int networksCount);
      static int structCmpBySignalQuality(const void *s1, const void *s2);
      String renderSelectOptionsHtml();
    public:
      String renderFormHtml();
};
