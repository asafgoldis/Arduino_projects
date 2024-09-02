//***************************************************************************************************************************//
//                                            Randum Light System V1.0
//***************************************************************************************************************************//
/*    IO Connaction:
// A0 - NOT CONNECTED
// A1 - NOT CONNECTED
// A2 - NOT CONNECTED
// A3 - NOT CONNECTED
// A4 - NOT CONNECTED
// A5 - NOT CONNECTED
// A6 - NOT CONNECTED
// A7 - NOT CONNECTED
// D0 - NOT CONNECTED 
// D1 - NOT CONNECTED
// D2 - RX_PIN
// D3 - TX_PIN
// D4 - NOT CONNECTED
// D5 - NOT CONNECTED
// D6 - NOT CONNECTED
// D7 - NOT CONNECTED
// D8 - NOT CONNECTED
// D9 - LED_R_PIN
// D10 - LED_G_PIN
// D11 - LED_B_PIN
// D12 - LED 2 PIN
// D13 - LED
*/
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

//***************************************************************************************************************************//
// Define IO
#define RX_PIN 2
#define TX_PIN 3

#define NUM_OF_OUTPUT 5
#define LED_B_PIN 9
#define LED_G_PIN 10
#define LED_R_PIN 11
#define LED_2_PIN 12
#define LED_PIN 13

//***************************************************************************************************************************//
const unsigned char outputPinArray[NUM_OF_OUTPUT] = {LED_PIN, LED_2_PIN, LED_R_PIN, LED_G_PIN, LED_B_PIN};
const unsigned int minTimeArrayOn[NUM_OF_OUTPUT] =  {2, 5   , 1   , 1 , 20};
const unsigned int maxTimeArrayOn[NUM_OF_OUTPUT] =  {2, 10  , 5   , 1 , 30};
const unsigned int minTimeArrayOff[NUM_OF_OUTPUT] = {2, 1*60, 60*5, 60, 5 };
const unsigned int maxTimeArrayOff[NUM_OF_OUTPUT] = {2, 3*60, 60*5, 60, 5 };

const unsigned int SpkMinTimeOff = 20*60;   // sec
const unsigned int SpkMaxTimeOff = 60*60;   // sec

unsigned long outputTimeArray[NUM_OF_OUTPUT];
unsigned long spkDelayTime = 0;

//***************************************************************************************************************************//
SoftwareSerial mySoftwareSerial(RX_PIN, TX_PIN); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//***************************************************************************************************************************//
void setup() {
  Serial.begin(9600);
  mySoftwareSerial.begin(9600);
  
  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));

  for (int i=0; i<NUM_OF_OUTPUT; i++) {
    pinMode(outputPinArray[i], OUTPUT);
    digitalWrite(outputPinArray[i], LOW);
    outputTimeArray[i] = 0;
  }

  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  myDFPlayer.volume(30);  //Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL); // Set different EQ
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD); // Set device we use SD as default

  delay(50);
}
//***************************************************************************************************************************//
void loop() {
  for (int i=0; i<NUM_OF_OUTPUT; i++) {
    UpdateOutput(i);
  }
  checkSpk();
  printStatus();

  delay(10);
}
//***************************************************************************************************************************//
void UpdateOutput(int i) {
  if (outputTimeArray[i] < millis()) {
    digitalWrite(outputPinArray[i], !digitalRead(outputPinArray[i]));
    if (digitalRead(outputPinArray[i]))
      outputTimeArray[i] = millis() + random(minTimeArrayOn[i], maxTimeArrayOn[i])*1000;
    else
      outputTimeArray[i] = millis() + random(minTimeArrayOff[i], maxTimeArrayOff[i])*1000;
  }
}
//***************************************************************************************************************************//
void checkSpk() {
  if (spkDelayTime < millis()) {
    int audioNum = random(1,myDFPlayer.readFileCounts()+1);     //read all file counts in SD card
    myDFPlayer.play(audioNum);
    spkDelayTime = millis() + random(SpkMinTimeOff, SpkMaxTimeOff)*1000;
    Serial.print("SPK ON, audio "); Serial.print(audioNum); Serial.print(", next audio "); Serial.print((spkDelayTime-millis())/1000); Serial.println("sec.");
  }
  
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
}
//***************************************************************************************************************************//
void printStatus() {
  for (int i=0; i<NUM_OF_OUTPUT; i++) {
    Serial.print("led "); Serial.print(i+1);
    if (digitalRead(outputPinArray[i])) Serial.print(" on , ");
    else                                Serial.print(" off, ");
  }
  Serial.println("");
}
//***************************************************************************************************************************//
void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
//***************************************************************************************************************************//
