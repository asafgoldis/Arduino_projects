//***************************************************************************************************************************//
//                                            Car Resistance Check V1.0
//***************************************************************************************************************************//

//    IO Connaction:
// A0 - INPUT_PIN_RX1
// A1 - INPUT_PIN_RX2
// A2 - INPUT_PIN_RX3
// A3 - INPUT_PIN_RX4
// A4 - I2C - SCL
// A5 - I2C - SDA
// A6 - INPUT_PIN_RX5
// A7 - CONSTANT_CURRENT__INPUT_PIN
// D0 - NOT CONNECTED
// D1 - NOT CONNECTED
// D2 - NOT CONNECTED
// D3 - NOT CONNECTED
// D4 - NOT CONNECTED
// D5 - NOT CONNECTED
// D6 - NOT CONNECTED
// D7 - SENSOR_RELAY_PIN_4
// D8 - SENSOR_RELAY_PIN_3
// D9 - SENSOR_RELAY_PIN_2
// D10 - SENSOR_RELAY_PIN_1
// D11 - START_BUTTON_PIN
// D12 - SCREEN_BUTTON_PIN
// D13 - NOT CONNECTED
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>   // make sure to change return value to 1 , in LiquidCrystal_I2C.cpp file of this library to avoid problem of only first character being displayed
//#include <Wire.h>
//#include <String.h>
//***************************************************************************************************************************//
// Define IO Pin
#define SENSOR_RELAY_PIN_1 10
#define SENSOR_RELAY_PIN_2 9
#define SENSOR_RELAY_PIN_3 8
#define SENSOR_RELAY_PIN_4 7

//#define POWER_PIN_RX1 6
//#define POWER_PIN_RX2 5
//#define POWER_PIN_RX3 4
//#define POWER_PIN_RX4 3
//#define POWER_PIN_RX5 2

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
const double MinXaxRes[2][NUM_OF_INPUT] = {2.0, 6.0, 2.0, 2.0, 40.0, 12.0, 24.0, 24.0, 5.2,
                                           0.9, 4.0, 1.0, 1.0,  5.0,  8.0, 18.0, 18.0, 3.5};
const String SensorName[9] = {"TPS1", "TPS2", "TISS", "TOSS", "OiTSol", "SOL_TCC", "SOL_A", "SOL_B", "OiPSol"};
int VoltageValue[NUM_OF_INPUT] = {0,0,0,0,0,0,0,0,0};
int TopLine = 0;

// Button Logic
const byte inputPins[NUM_OF_BUTTONS] = {START_BUTTON_PIN, SCREEN_BUTTON_PIN};
byte inputState[NUM_OF_BUTTONS];
byte lastInputState[NUM_OF_BUTTONS] = {HIGH,HIGH};
bool inputFlags[NUM_OF_BUTTONS] = {LOW,LOW};
unsigned long lastDebounceTime[NUM_OF_BUTTONS] = {0,0};
#define DEBOUNCE_DELAY 5

LiquidCrystal_I2C lcd(LiquidCrystal_I2C_ADDRESS,20,4); // put here correct address of lcd...  if not known, use Arduino I2CScanner given in above link
//***************************************************************************************************************************//
void setup() {
  Serial.begin(9600); //just in case if you want to get date and time in serial port
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.home();
  // Serial.println("System start");
  
  // analogReference(EXTERNAL);  // Vref connect to 3.3V pin
  pinMode(SENSOR_RELAY_PIN_1, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_1, HIGH);
  pinMode(SENSOR_RELAY_PIN_2, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_2, HIGH);
  pinMode(SENSOR_RELAY_PIN_3, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_3, HIGH);
  pinMode(SENSOR_RELAY_PIN_4, OUTPUT); digitalWrite(SENSOR_RELAY_PIN_4, HIGH);
/*
  pinMode(POWER_PIN_RX1, OUTPUT);      digitalWrite(POWER_PIN_RX1, LOW);
  pinMode(POWER_PIN_RX2, OUTPUT);      digitalWrite(POWER_PIN_RX2, LOW);
  pinMode(POWER_PIN_RX3, OUTPUT);      digitalWrite(POWER_PIN_RX3, LOW);
  pinMode(POWER_PIN_RX4, OUTPUT);      digitalWrite(POWER_PIN_RX4, LOW);
  pinMode(POWER_PIN_RX5, OUTPUT);      digitalWrite(POWER_PIN_RX5, LOW);
*/
  for(int i=0; i<NUM_OF_BUTTONS; i++)   // set button's pin
    pinMode(inputPins[i], INPUT_PULLUP);
  
  lcd.clear();                      // clear the lcd
  lcd.setCursor(0, 0); lcd.print("Gear Diagnosis"); // column 0 , row 0 i.e. top line of LCD
  lcd.setCursor(0, 1); lcd.print("System V1.0 Made By:");
  lcd.setCursor(0, 2); lcd.print("--- ASAF GOLDIS ---");
  lcd.setCursor(0, 3); lcd.print("Press \"Start\" ");
  delay(1000);
  
  lcd.print("Ready!");
  Serial.println("System Ready!!!");
}
//***************************************************************************************************************************//
void loop() {
  setInputFlags();
  resolveInputFlags();
  
  delay(20);
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
  for(int i=0; i<NUM_OF_BUTTONS; i++) {
    if(inputFlags[i] == HIGH) {
      inputAction(i);
      inputFlags[i] = LOW;
    }
  }
}
//***************************************************************************************************************************//
void inputAction(int Button) {
  lcd.clear();
  if (Button == 0) {  // start
    lcd.setCursor(0, 0);  // column 0 , row 0
    lcd.print("Wait..... (25 sec)");
    lcd.setCursor(0,1);   // column 0 , row 1
    lcd.print("Processing...");
    TopLine = 0;
    unsigned long Time = millis();
    for (int i=0; i<NUM_OF_INPUT; i++)
      VoltageValue[i] = CheckValue(i+1, Time);
    // Serial.println(millis()-Time);
  }
  else if (Button == 1) {  // down
    TopLine++;
    if (TopLine == MAX_LINE) TopLine = 0;
  }
  printScreen();
}
//***************************************************************************************************************************//
void printScreen() {
  int line[4];
  int Length[4];
  double Resistance[4] = {0.0, 0.0, 0.0, 0.0};
  String lineMsg[4] = {"", "", "", ""};
  
  byte ohm[8] = {0x00, 0x0E, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x1B};
  byte OK[8] = {0x01, 0x01, 0x03, 0x02, 0x12, 0x0A, 0x0C, 0x04};
  byte NOT_OK[8] = {0x11, 0x0A, 0x0A, 0x04, 0x04, 0x0A, 0x0A, 0x11};

  lcd.createChar(1, ohm);
  lcd.createChar(2, OK);
  lcd.createChar(3, NOT_OK);
  
  line[0] = TopLine;  
  for (int i=0; i<4; i++) {
    line[i+1] = line[i] + 1;
    if (line[i+1] == MAX_LINE) line[i+1] -= MAX_LINE;
    Resistance[i] = CheckResistance(VoltageValue[line[i]], line[i]+1);
    lineMsg[i] += SensorName[line[i]];
    if (lineMsg[i].length()!=7) {
      for (int j=7-lineMsg[i].length(); j>0; j--)
        lineMsg[i] += ".";
    }
    lineMsg[i] += "; "; lineMsg[i] += Resistance[i]; lineMsg[i] += " ";
    if (line[i]<=5) lineMsg[i] += "K";
    Length[i] = lineMsg[i].length();
    lineMsg[i] += " ";
  }
  Serial.println(".");
  lcd.clear();
  for (int i=0; i<4; i++) {
    lcd.setCursor(0,i);    // column 0 , row i
    lcd.print(lineMsg[i]);
    lcd.setCursor(Length[i],i);
    lcd.write(1);

    lcd.setCursor(19,i);
    if (MinXaxRes[1][line[i]]<=Resistance[i] && Resistance[i]<=MinXaxRes[0][line[i]]) lcd.write(2);  // min<=Resistance[i]<=max
    else                                                                              lcd.write(3);
  }
    
}
//***************************************************************************************************************************//
int CheckValue(int resNum, unsigned long StartTime) {
  const unsigned long runTime = 25189;
  double Time = 100.0*(millis()-StartTime)/runTime;
  int input_pin_num = 0;
  unsigned int sensorValue = 0;

  if      (resNum == 1)  input_pin_num = INPUT_PIN_RX1;
  else if (resNum == 2)  input_pin_num = INPUT_PIN_RX2;
  else if (resNum == 3)  input_pin_num = INPUT_PIN_RX3;
  else if (resNum == 4)  input_pin_num = INPUT_PIN_RX4;
  else if (resNum == 5)  input_pin_num = INPUT_PIN_RX5;
  else                   input_pin_num = CONSTANT_CURRENT__INPUT_PIN;

  lcd.setCursor(0,2); lcd.print("Checking Sensor "); lcd.print(resNum);
  lcd.setCursor(0,3); lcd.print(Time); lcd.print("% Complete"); 
  ManegeInput(resNum);
  
  for (int i=0; i<30; i++) {
    if (i%2==0) Time = 100.0*(millis()-StartTime)/runTime;
    lcd.setCursor(0,3); lcd.print(Time); lcd.print("% Complete"); 
    sensorValue += analogRead(input_pin_num);  // variable to store the value coming from the sensor
    delay(50);
  }
  unsigned int AvgVal = sensorValue/30;    // variable to store the value coming from the sensor
  //Serial.print(resNum); Serial.print("; AvgVal: "); Serial.println(AvgVal);
  
  ManegeInput(0); // close all input
  Time = 100.0*(millis()-StartTime)/runTime;
  lcd.setCursor(0,3); lcd.print(Time); lcd.print("% Complete");
    
  return AvgVal;
}
//***************************************************************************************************************************//
void  ManegeInput(int InputNum) {
  digitalWrite(SENSOR_RELAY_PIN_1, HIGH); digitalWrite(SENSOR_RELAY_PIN_2, HIGH); digitalWrite(SENSOR_RELAY_PIN_3, HIGH); digitalWrite(SENSOR_RELAY_PIN_4, HIGH);
  //digitalWrite(POWER_PIN_RX1, LOW); digitalWrite(POWER_PIN_RX2, LOW); digitalWrite(POWER_PIN_RX3, LOW); digitalWrite(POWER_PIN_RX4, LOW); digitalWrite(POWER_PIN_RX5, LOW); 
  /*
  if      (InputNum == 1) digitalWrite(POWER_PIN_RX1, HIGH);
  else if (InputNum == 2) digitalWrite(POWER_PIN_RX2, HIGH);
  else if (InputNum == 3) digitalWrite(POWER_PIN_RX3, HIGH);
  else if (InputNum == 4) digitalWrite(POWER_PIN_RX4, HIGH);
  else if (InputNum == 5) digitalWrite(POWER_PIN_RX5, HIGH);

  else */ if (InputNum == 6) digitalWrite(SENSOR_RELAY_PIN_1, LOW);
  else if (InputNum == 7) digitalWrite(SENSOR_RELAY_PIN_2, LOW);
  else if (InputNum == 8) digitalWrite(SENSOR_RELAY_PIN_3, LOW);
  else if (InputNum == 9) digitalWrite(SENSOR_RELAY_PIN_4, LOW);

  delay(250);
}
//***************************************************************************************************************************//
double CheckResistance(int Value, int resNum) {
  double voltage = REF_VOL*Value/1023.0;
  double Res = 0.0;

  if      (resNum == 1) Res = Value*R1/(1024.0-Value);
  else if (resNum == 2) Res = Value*R2/(1024.0-Value);
  else if (resNum == 3) Res = Value*R3/(1024.0-Value);
  else if (resNum == 4) Res = Value*R4/(1024.0-Value);
  else if (resNum == 5) Res = Value*R5/(1024.0-Value);
  else                  Res = Value*Rref;
  
  Serial.print(resNum); Serial.print("; Value: "); Serial.print(Value); Serial.print(", Voltage: "); Serial.print(voltage); Serial.print("[V], Res: "); Serial.print(Res);
  if (resNum <= 5) Serial.println("[Kohm]");
  else             Serial.println("[ohm]");
    
  return Res;
}
//***************************************************************************************************************************//
