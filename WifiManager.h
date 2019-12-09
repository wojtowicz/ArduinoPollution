class WifiManager {
  private:
    String ssid;
    String password;
    boolean connectWifi;
    boolean wifiConnecting;
    WifiManager();
    void displayWifiInfo();
    void startWifiAP();
    void displayMessageOnLCD(String message, int row);
    void handleWifiConnected();
  public:
    static WifiManager& getInstance();
    void clear();
    void setup();
    void connect();
    void setSSID(String ssid);
    void setPassword(String password);
    String getSSID();
    String getPassword();
    boolean isConnectWifi();
    boolean isWifiConnecting();
    void setConnectWifiToOn();
    void setConnectWifiToOff();
    void setWifiConnectingToOn();
    void setWifiConnectingToOff();
    void handleWifiConnection();
};
