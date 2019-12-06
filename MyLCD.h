#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

class MyLCD {
    public:
      void configure();
      void clear();
      void display(struct LCDMessageStruct lcdMessage);
};
