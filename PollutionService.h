class PollutionService {
    public:
      void setup();
      void fetch();
      void ledsOff();
    private:
      void parse();
      void displayOnLCD();
      void blinkLeds();
};
