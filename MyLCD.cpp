#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>

#include "LCDMessageStruct.h"
#include "MyLCD.h"

LiquidCrystal_PCF8574 lcd(0x27);

void MyLCD::configure() {
  int error;
  
  Serial.println("Configuring LCD...");
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");
    lcd.begin(16, 2);
    lcd.setBacklight(255);
    lcd.home();
    lcd.clear();
  } else {
    Serial.println(": LCD not found.");
  }
}

void MyLCD::clear(){
  lcd.clear();
}

void MyLCD::display(LCDMessageStruct lcdMessage) {
  if(lcdMessage.debug){
    Serial.print(lcdMessage.debugLabel);
    Serial.print(lcdMessage.content);
  }
  lcd.setCursor(lcdMessage.col, lcdMessage.row);
  lcd.print(lcdMessage.content);
}
