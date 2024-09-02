//***************************************************************************************************************************//
//                                            Remote Control System - HONAAA V1.0
//***************************************************************************************************************************//

//    IO Connaction:
// A0 - MICROSWISH_2_PIN  // for motor 1
// A1 - MICROSWISH_3_PIN  // for motor 2
// A2 - MICROSWISH_4_PIN  // for motor 2
// A3 - NOT CONNECTED
// A4 - NOT CONNECTED
// A5 - NOT CONNECTED
// A6 - NOT CONNECTED
// A7 - NOT CONNECTED
// D0 - NOT CONNECTED 
// D1 - NOT CONNECTED
// D2 - TX
// D3 - RX
// D4 - switch_relay_4
// D5 - switch_relay_3
// D6 - switch_relay_2
// D7 - switch_relay_1
// D8 - motor_relay_4
// D9 - motor_relay_3
// D10 - motor_relay_2
// D11 - motor_relay_1
// D12 - MICROSWISH_1_PIN  // for motor 1
// D13 - NOT CONNECTED
//***************************************************************************************************************************//
#include <Wire.h>
//#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>   // make sure to change return value to 1 , in LiquidCrystal_I2C.cpp file of this library to avoid problem of only first character being displayed
#include <String.h>
//***************************************************************************************************************************//
// Define IO Pin
#define SENSOR_RELAY_PIN_1 10
#define SENSOR_RELAY_PIN_2 9
#define SENSOR_RELAY_PIN_3 8
#define SENSOR_RELAY_PIN_4 7

#define POWER_PIN_RX1 6
#define POWER_PIN_RX2 5
#define POWER_PIN_RX3 4
#define POWER_PIN_RX4 3
#define POWER_PIN_RX5 2

#define INPUT_PIN_RX1 A0
#define INPUT_PIN_RX2 A1
#define INPUT_PIN_RX3 A2
#define INPUT_PIN_RX4 A3
#define INPUT_PIN_RX5 A6
#define CONSTANT_CURRENT__INPUT_PIN A7

#define START_BUTTON_PIN 11
#define SCREEN_BUTTON_PIN 12

// Define constant
#define NUM_OF_INPUT 9
#define MAX_LINE 9
#define NUM_OF_BUTTONS 2

#define REF_VOL 5.00  //Volt
#define MY_CONSTANT_CURRENT 0.121 // Amp

//#define Rc 10.41  //ohm   Rc=1.25/I; I=0.121A
#define R1 1.333   //Kohm
#define R2 4.70    //Kohm
#define R3 1.466   //Kohm
#define R4 1.471   //Kohm
#define R5 14.74   //Kohm

#define LiquidCrystal_I2C_ADDRESS 0x27

const double Rref = REF_VOL/(1023.0*MY_CONSTANT_CURRENT);
const double MinXaxRes[2][NUM_OF_INPUT] = {  2, 6, 2, 2, 40, 12, 24, 24, 5.2,
                                           0.9, 4, 1, 1,  5,  8, 18, 18, 3.5};
const String SensorName[9] = {"TPS1", "TPS2", "TISS", "TOSS", "OiTSol", "SOL_TCC", "SOL_A", "SOL_B", "OiPSol"};

// #define MY_RES_HIGH 14.0   //Kohm
// #define MY_RES_LOW 2.3   //Kohm

// #define R_MAX 40      //Kohm
// #define R_MIN 0.9     //Kohm

//#define SENSOR_PIN A0  // select the input pin for the potentiometer
//#define LED_PIN 13    // select the pin for the LED
//***************************************************************************************************************************//
LiquidCrystal_I2C lcd(LiquidCrystal_I2C_ADDRESS,20,4); // put here correct address of lcd...  if not known, use Arduino I2CScanner given in above link

//String scrollingmessage = "                 Using Arduino Due internal RTC, no need of external RTC chip. Have fun.... :)        ";

// int sensorMin = 1023;        // minimum sensor value
// int sensorMax = 0;           // maximum sensor value

//double Resistance[9] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
int VoltageValue[NUM_OF_INPUT] = {0,0,0,0,0,0,0,0,0};
// int j = 0;

int TopLine = 0;
// int ResNum = 0;


//Input & Button Logic
const byte inputPins[NUM_OF_BUTTONS] = {START_BUTTON_PIN, SCREEN_BUTTON_PIN};
byte inputState[NUM_OF_BUTTONS];
byte lastInputState[NUM_OF_BUTTONS] = {HIGH,HIGH};
bool inputFlags[NUM_OF_BUTTONS] = {LOW,LOW};
unsigned long lastDebounceTime[NUM_OF_BUTTONS] = {0,0};
#define DEBOUNCE_DELAY 5


//***************************************************************************************************************************//
void setup() {
  Serial.begin(9600); //just in case if you want to get date and time in serial port
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home();
  Serial.println("system start");
//  unsigned int Xmin = 1024*R_MIN/(R_MIN+MY_RES);
//  unsigned int Xmax = 1024*(R_MAX/(R_MAX+MY_RES));
//  Serial.print("Xmin: "); Serial.print(Xmin); Serial.print(",  Xmax: "); Serial.println(Xmax);
  
  
 //analogReference(EXTERNAL);  // Vref connect to 3.3V pin
  
//  pinMode(LED_PIN, OUTPUT);
//  digitalWrite(LED_PIN, HIGH);
/*
  DDRB = B111111;   // define pin 8,9,10,11,12,13 output
  DDRD = B11111110; // define pin 0 as input and pin 1,2,3,4,5,6,7 output
  
*/

  pinMode(SENSOR_RELAY_PIN_1, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_1, HIGH);
  pinMode(SENSOR_RELAY_PIN_2, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_2, HIGH);
  pinMode(SENSOR_RELAY_PIN_3, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_3, HIGH);
  pinMode(SENSOR_RELAY_PIN_4, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_4, HIGH);
  pinMode(POWER_PIN_RX1, OUTPUT);      digitalWrite(POWER_PIN_RX1, LOW);
  pinMode(POWER_PIN_RX2, OUTPUT);      digitalWrite(POWER_PIN_RX2, LOW);
  pinMode(POWER_PIN_RX3, OUTPUT);      digitalWrite(POWER_PIN_RX3, LOW);
  pinMode(POWER_PIN_RX4, OUTPUT);      digitalWrite(POWER_PIN_RX4, LOW);
  pinMode(POWER_PIN_RX5, OUTPUT);      digitalWrite(POWER_PIN_RX5, LOW);

  for(int i=0; i<NUM_OF_BUTTONS; i++)   // set button's pin
    pinMode(inputPins[i], INPUT_PULLUP);
  
  lcd.clear();                      // clear the lcd
  lcd.setCursor(0, 0);  // column 0 , row 0 i.e. top line of LCD
  lcd.print("Measurement's System");
  
  lcd.setCursor(0, 1);  // column 0 , row 1
  lcd.print("by ASAF GOLDIS");
  
  lcd.setCursor(0, 2);  // column 0 , row 2
  lcd.print("Press \"Start\" button");
  
  delay(2000);
  lcd.setCursor(0, 3);  // column 0 , row 3
  lcd.print("Ready!!!");
  Serial.println("System Ready!!!");
}
//***************************************************************************************************************************//
void loop() {
  setInputFlags();
  resolveInputFlags();
  /*
  ResNum++;
  if (ResNum > NUM_OF_INPUT) {
    ResNum = 1;
  //lcd.clear();
    lcd.home();
  }
  
  ResNum = 1;
  int Val = CheckValue(ResNum);
  double Resistance = CheckResistance(Val, ResNum);
  
  lcd.setCursor(0, ResNum);  // column 0 , row j
  lcd.print(ResNum); lcd.print("; "); lcd.print(Resistance);
  // lcd.print(" Kohm");
  if (ResNum <= 5) lcd.print("[Kohm]");
  else             lcd.print("[ohm]");
  */
  /*
  lcd.clear();
  lcd.home();
  
  lcd.setCursor(0, 0);                  // column 1 , row 0
  lcd.print(j); lcd.print("; Resistance: ");
  
  lcd.setCursor(0, 1);
  lcd.print(Resistance);  // 
  lcd.print(" Kohm");
  */
  delay(100);
}
//***************************************************************************************************************************//
void setInputFlags() {
  for(int i=0; i<NUM_OF_BUTTONS; i++) {
    int reading = digitalRead(inputPins[i]);
    if (reading != lastInputState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis()-lastDebounceTime[i]) > DEBOUNCE_DELAY) {
      if (reading != inputState[i]) {
        inputState[i] = reading;
        if (inputState[i] == LOW) inputFlags[i] = HIGH;
      }
    }
    lastInputState[i] = reading;
  }
}
//***************************************************************************************************************************//
void resolveInputFlags() {
  //bool change = false;
  for(int i=0; i<NUM_OF_BUTTONS; i++) {
    if(inputFlags[i] == HIGH) {
      //change = true;
      // last_time_change = millis();
      inputAction(i);
      inputFlags[i] = LOW;
      //printScreen();
    }
  }
  
  //if (change == false && ROOT_SCREEN == 0)
  //  printScreen();
}
//***************************************************************************************************************************//
void inputAction(int Button) {
  //Serial.println("inputAction()");
  lcd.clear();
  //Serial.print("Button: "); Serial.println(Button);
  if (Button == 0) {  // start
    lcd.setCursor(0, 0);  // column 0 , row 0
    lcd.print("Wait..... (20 sec)");
    TopLine = 0;
    for (int i=0; i<NUM_OF_INPUT; i++)
      VoltageValue[i] = CheckValue(i+1);
  }
  else if (Button == 1) {  // down
    TopLine++;
    if (TopLine == MAX_LINE) TopLine = 0;
    // printScreen();
  }
  printScreen();
}
//***************************************************************************************************************************//
void printScreen() {
  Serial.println("printScreen()");
  int line[4];
  double Resistance[4] = {0.0, 0.0, 0.0, 0.0};
  String lineMsg[4] = {"", "", "", ""};
  
    
  byte ohm[8] = {0b00100,
                 0b01100,
                 0b11111,
                 0b01101,
                 0b00101,
                 0b00001,
                 0b00001,
                 0b11111 };
  lcd.createChar(1, ohm);
  // lcd.write(1);
  //Serial.print("ohm"); Serial.println(ohm);
  
  line[0] = TopLine;
  //line[1] = line[0] + 1; if (line[1] == MAX_LINE) line[1] -= MAX_LINE;
  //line[2] = line[1] + 1; if (line[2] == MAX_LINE) line[2] -= MAX_LINE;
  //line[3] = line[2] + 1; if (line[3] == MAX_LINE) line[3] -= MAX_LINE;
  
  for (int i=0; i<4; i++) {
    line[i+1] = line[i] + 1;
    if (line[i+1] == MAX_LINE)
      line[i+1] -= MAX_LINE;
  }
  
  for (int i=0; i<4; i++)
    Resistance[i] = CheckResistance(VoltageValue[line[i]], line[i]+1);
  
    
  // lcd.write(1);
  for (int i=0; i<4; i++) {
    lineMsg[i] += SensorName[line[i]]; lineMsg[i] += "; ";
    lineMsg[i] += Resistance[i];
    lineMsg[i] += " ["; if (line[i]<=5) lineMsg[i] += "K"; lineMsg[i] += "ohm]";
  
    if (MinXaxRes[1][line[i]]<=Resistance[i] && Resistance[i]<=MinXaxRes[0][line[i]]) lineMsg[i] += " OK";  // min<=Resistance[i]<=max
    else                                                                              lineMsg[i] += "BAD";
  }
  
  for (int i=0; i<4; i++) {
    lcd.setCursor(0,i);    // column 0 , row i
    lcd.print(lineMsg[i]);
  }
    
}
//***************************************************************************************************************************//
int CheckValue(int resNum) {
  ManegeInput(resNum);

  int input_pin_num = 0;
  if      (resNum == 1)  input_pin_num = INPUT_PIN_RX1;
  else if (resNum == 2)  input_pin_num = INPUT_PIN_RX2;
  else if (resNum == 3)  input_pin_num = INPUT_PIN_RX3;
  else if (resNum == 4)  input_pin_num = INPUT_PIN_RX4;
  else if (resNum == 5)  input_pin_num = INPUT_PIN_RX5;
  else                   input_pin_num = CONSTANT_CURRENT__INPUT_PIN;
  
  unsigned int sensorValue = 0;
  for (int i=0; i<30; i++) {
    sensorValue += analogRead(input_pin_num);  // variable to store the value coming from the sensor
    delay(50);
  }
  unsigned int AvgVal = sensorValue/30;    // variable to store the value coming from the sensor
  // if      (AvgVal > 914) AvgVal = 1024;
  // else if (AvgVal < 110) AvgVal = 0;
  Serial.print(resNum); Serial.print("; AvgVal: "); Serial.println(AvgVal);
  ManegeInput(0); // close all input
  
  return AvgVal;
}
//***************************************************************************************************************************//
void  ManegeInput(int InputNum) {
  digitalWrite(SENSOR_RELAY_PIN_1, HIGH); digitalWrite(SENSOR_RELAY_PIN_2, HIGH); digitalWrite(SENSOR_RELAY_PIN_3, HIGH); digitalWrite(SENSOR_RELAY_PIN_4, HIGH);
  digitalWrite(POWER_PIN_RX1, LOW); digitalWrite(POWER_PIN_RX2, LOW); digitalWrite(POWER_PIN_RX3, LOW); digitalWrite(POWER_PIN_RX4, LOW); digitalWrite(POWER_PIN_RX5, LOW); 
  //delay(100);
  if      (InputNum == 1) digitalWrite(POWER_PIN_RX1, HIGH);
  else if (InputNum == 2) digitalWrite(POWER_PIN_RX2, HIGH);
  else if (InputNum == 3) digitalWrite(POWER_PIN_RX3, HIGH);
  else if (InputNum == 4) digitalWrite(POWER_PIN_RX4, HIGH);
  else if (InputNum == 5) digitalWrite(POWER_PIN_RX5, HIGH);
  else if (InputNum == 6) digitalWrite(SENSOR_RELAY_PIN_1, LOW);
  else if (InputNum == 7) digitalWrite(SENSOR_RELAY_PIN_2, LOW);
  else if (InputNum == 8) digitalWrite(SENSOR_RELAY_PIN_3, LOW);
  else if (InputNum == 9) digitalWrite(SENSOR_RELAY_PIN_4, LOW);

  delay(250);
}
//***************************************************************************************************************************//
double CheckResistance(int Value, int resNum) {
  Serial.println("CheckResistance()");
  double voltage = REF_VOL*Value/1023.0;
  double Res = 0.0;

  //const double VVref = 1024.0*REF_VOL/5.0;
  
  // double myRes = 0.0;
  // double correction = 0.0;
  
  
  if      (resNum == 1) Res = Value*R1/(1024.0-Value);
  else if (resNum == 2) Res = Value*R2/(1024.0-Value);
  else if (resNum == 3) Res = Value*R3/(1024.0-Value);
  else if (resNum == 4) Res = Value*R4/(1024.0-Value);
  else if (resNum == 5) Res = Value*R5/(1024.0-Value);
  else                  Res = Value*Rref;
/*
  if      (resNum == 1) Res = Value*R1/(VVref-Value);
  else if (resNum == 2) Res = Value*R2/(VVref-Value);
  else if (resNum == 3) Res = Value*R3/(VVref-Value);
  else if (resNum == 4) Res = Value*R4/(VVref-Value);
  else if (resNum == 5) Res = Value*R5/(VVref-Value);
  else                  Res = Value*Rref;
  // correction = (0.282 +0.186*voltage);
  // Res = myRes*Value/(1023.0-Value);
  // Res -= correction;
  */
  Serial.print(resNum); Serial.print("; Value: "); Serial.print(Value); Serial.print(", Voltage: "); Serial.print(voltage); Serial.print("[V], Res: "); Serial.print(Res);
  if (resNum <= 5) Serial.println("[Kohm]");
  else             Serial.println("[ohm]");
  
  return Res;
}
//***************************************************************************************************************************//
/*
void Scrollingmessage(String MSG) {
  int ii = 0;
  int strLength = scrollingmessage.length();;
  String toShow;

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
   
    toShow = MSG.substring(ii,ii+20);   // Get 20 characters so that we can display on the LCD
    lcd.print(toShow);  // print the number of seconds since reset:
  
    ii = ii + 1;  // move 1 step left
    if(ii>(strLength-20)) {   // We have to reset ii after there is less text displayed.
      ii = 0;
    }

    delay(210);  // set this as per the required speed of scrolling
  }  // end of for loop
}
*/
