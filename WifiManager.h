class WifiManager {
  private:
    String ssid;
    String password;
    boolean connectWifi;
    boolean disconnectWifi;
    boolean wifiConnecting;
    WifiManager();
    void saveConfig();
    void displayWifiInfo();
    void startWifiAP();
    void displayMessageOnLCD(String message, int row);
    void handleWifiConnected();
  public:
    static WifiManager& getInstance();
    void clear();
    void setup();
    void connect();
    void disconnect();
    void setSSID(String ssid);
    void setPassword(String password);
    String getSSID();
    String getPassword();
    boolean isConnectWifi();
    boolean isDisconnectWifi();
    boolean isWifiConnecting();
    void setConnectWifiToOn();
    void setConnectWifiToOff();
    void setDisconnectWifiToOn();
    void setDisconnectWifiToOff();
    void setWifiConnectingToOn();
    void setWifiConnectingToOff();
    void handleWifiConnection();
};
