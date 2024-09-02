// Following code by Electromania is based on...
// https://github.com/build2master/arduino-lcd-scroll-long-text/blob/master/arduino-lcd-scroll-long-text.ino  for scrolling text concept
// https://github.com/MarkusLange/Arduino-Due-RTC-Library  for RTC library
// https://github.com/marcoschwartz/LiquidCrystal_I2C   for Arduino due library for LCD
// http://playground.arduino.cc/Main/I2cScanner  for Arduino I2C address scanner sketch
// Thanks to these people for their previous work....

// The demo of this code is available on my youtube channel-  https://www.youtube.com/watch?v=SPk8xxSgUxk  

#include <rtc_clock.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // make sure to change return value to 1 , in LiquidCrystal_I2C.cpp file of this library to avoid problem of only first character being displayed

RTC_clock rtc_clock(XTAL);
LiquidCrystal_I2C lcd(0x27,16,2); // put here correct address of lcd...  if not known, use Arduino I2CScanner given in above link

char* daynames[]={"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
int hh,mm,ss,dow,dd,mon,yyyy;

String scrollingmessage = "                 Using Arduino Due internal RTC, no need of external RTC chip. Have fun.... :)        ";
int ii = 0;
int strLength;
String toShow;

void setup() {

  //Serial.begin(9600); just in case if you want to get date and time in serial port
  rtc_clock.init();
  rtc_clock.set_clock(__DATE__, __TIME__);
  //---------------------------------------Optional part begin--------------------------------------------
  // This optional part can be removed later to show only time and date, I added this section here just as fun and to experiment with scrolling text
  strLength = scrollingmessage.length();
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home();
  lcd.setCursor(0, 0);  // column 0 , row 0 i.e. top line of LCD
  lcd.print("Electromania....");
  delay(200);
  lcd.setCursor(0, 1);  // column , row  column 0 , row 1 i.e. bottom line of LCD
  lcd.print("Fun with Arduino :)");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);  // column , row 0
  lcd.print("Clock & Calendar");
  lcd.setCursor(0, 1);  // column , row 1

  for (int timeCounter = 0; timeCounter < 90; timeCounter++) { // this loop is to wait until whole scrolling message is displayed

  lcd.home();
  lcd.setCursor(0, 1);  // column , row 1
 
  toShow = scrollingmessage.substring(ii,ii+16);   // Get 16 characters so that we can display on the LCD
  
  lcd.print(toShow);  // print the number of seconds since reset:

  ii = ii + 1;  // move 1 step left

   if(ii>(strLength-16)) {   // We have to reset ii after there is less text displayed.
   ii = 0;
  }

  delay(210);  // set this as per the required speed of scrolling
}  // end of for loop

//--------------------------Optional part ends here---------------------------------------------------------

  lcd.clear();                      // ckear the lcd 
  
  // here set time and date is used by me to start clock at given time, if we comment following two lines, it will take actual time from your PC
  rtc_clock.set_time(23, 59, 56);  // set RTC starting time ...  later buttons can be used to set the time settings  
  rtc_clock.set_date(31, 12,2015);  //set RTC starting date ...  later buttons can be used to set the date settings
}



void loop() {
  rtc_clock.get_time(&hh,&mm,&ss);  // get time from Arduino RTC
  rtc_clock.get_date(&dow,&dd,&mon,&yyyy);   // get date from Arduino RTC
  lcd.home();
  lcd.setCursor(1, 0);                  // column 1 , row 0
  lcd.print("Time: ");
  
  // Print time
  if(hh <10) {lcd.print("0"); }      // to display in 01,02,03... format
    lcd.print(hh);                      // display hours
    lcd.print(":");

  if(mm <10) {lcd.print("0"); }
    lcd.print(mm);                     // display minutes
    lcd.print(":");
    
  if(ss <10) {lcd.print('0'); }
  lcd.print(ss);                       // display seconds

  // Print day and date 
  lcd.setCursor(1, 1);
  lcd.print(daynames[dow-1]);  // display Day name
  lcd.print(" ");
  if(dd <10) {lcd.print("0"); }  
  lcd.print(dd);                // display date
  lcd.print("/");
  if(mm <10) {lcd.print("0"); }
  lcd.print(mon);              // display month
  lcd.print("/");
  lcd.print(yyyy);            // display year
 
}

