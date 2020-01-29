class PollutionService {
    public:
      void setup();
      void fetch(String uuid);
      void ledsOff();
    private:
      void parse();
      void displayOnLCD();
      void blinkLeds();
};
