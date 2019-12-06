class PollutionService {
    public:
      void setup();
      void fetch();
    private:
      void parse();
      void displayOnLCD();
      void blinkLeds();
};
