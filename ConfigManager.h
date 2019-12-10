class ConfigManager {
  private:
    String ssid;
    String pass;
    ConfigManager();
  public:
    static ConfigManager& getInstance();
    bool load();
    bool save();
    bool reset();
    bool isDeviceConfigured();
    String getSSID();
    void setSSID(String ssid);
    String getPassword();
    void setPassword(String password);
};
