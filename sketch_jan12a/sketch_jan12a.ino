#include <LiquidCrystal_I2C.h>



//create an object called lcd, with address 0x38, wich is a display with 4 lines and 20 chars per line

LiquidCrystal_I2C lcd(0x38,20,4);

void setup()

{

lcd.init();

lcd.setBacklight(LOW);

lcd.print("20x4 LCD I2C adapter");

}

void loop()

{

lcd.setCursor(0,2);

lcd.print(" @instructables.com");

}
