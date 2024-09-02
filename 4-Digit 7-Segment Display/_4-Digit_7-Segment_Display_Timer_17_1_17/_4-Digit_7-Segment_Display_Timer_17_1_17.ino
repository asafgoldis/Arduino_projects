
#include "SevSeg.h"
SevSeg sevseg; //Instantiate a seven segment controller object
int resetbutten =A0;

void setup() {
  byte numDigits = 4;   
  byte digitPins[] = {2, 3, 4, 5};
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  bool resistorsOnSegments = false; // Use 'true' if on digit pins
  byte hardwareConfig = COMMON_CATHODE; // See README.md for options
  
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(50);
  pinMode(resetbutten, INPUT);
}

void loop() {
  static unsigned long timer = millis();
// static int deciSeconds = 0; //up
  static int deciSeconds = 10000; //down


      if (millis() >= timer) {
//        deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond//up
        deciSeconds--; // 100 milliSeconds is equal to 1 deciSecond//down

        timer += 100;
//        if (deciSeconds == 10000) { // Reset to 0 after counting for 1000 seconds.//up
       if (deciSeconds == 0) { // Reset to 1000 after counting for 1000 seconds.//down          
//          deciSeconds=10000;//up
          deciSeconds=10000;//down
        }
        if (analogRead(resetbutten)>1000) {
//          deciSeconds=0; //up
          deciSeconds=10000; //down
        }
        sevseg.setNumber(deciSeconds, 1);
      }
  
  sevseg.refreshDisplay(); // Must run repeatedly
}
  

/// END ///
