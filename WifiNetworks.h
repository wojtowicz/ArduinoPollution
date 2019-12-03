#include <ESP8266WiFi.h>

class WifiNetworks {
    private:
      int32_t getSignalQuality(int32_t signalStrength);
      String getEncryptionType(uint8_t wifiEcryptionType);
      void buildWifiNetworks(struct WifiNetwork *wifiNetworks, int networksCount);
      void sortWifiNetworks(struct WifiNetwork *wifiNetworks, int networksCount);
      String getSelectOptionsHtml(struct WifiNetwork *wifiNetworks, int networksCount);
      static int structCmpBySignalQuality(const void *s1, const void *s2);
      String renderSelectOptionsHtml();
    public:
      String renderFormHtml();
};
