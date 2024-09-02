//***************************************************************************************************************************//
//                                            Car Resistance Check V1.2
//***************************************************************************************************************************//


// החלפה של ההזנות לריללי 8 מקומות במקום 4 כדי למנוע מדידה של נגדים במקביל
// עדכון של ערכי הנגדים

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
// D2 - START_BUTTON_PIN
// D3 - SCREEN_BUTTON_PIN
// D4 - SENSOR_RELAY_PIN_4
// D5 - SENSOR_RELAY_PIN_3
// D6 - SENSOR_RELAY_PIN_2
// D7 - SENSOR_RELAY_PIN_1
// D8 - NOT CONNECTED
// D9 - NOT CONNECTED
// D10 - NOT CONNECTED
// D11 - NOT CONNECTED
// D12 - NOT CONNECTED
// D13 - NOT CONNECTED
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>   // make sure to change return value to 1 , in LiquidCrystal_I2C.cpp file of this library to avoid problem of only first character being displayed
//***************************************************************************************************************************//
// Define IO Pin
#define CONSTANT_CURRENT_RELAY_PIN_1 7
#define CONSTANT_CURRENT_RELAY_PIN_2 6
#define CONSTANT_CURRENT_RELAY_PIN_3 5
#define CONSTANT_CURRENT_RELAY_PIN_4 4

#define VOLTAGE_DIVIDER_POWER_PIN_1 13
#define VOLTAGE_DIVIDER_RELAY_PIN_2 12
#define VOLTAGE_DIVIDER_RELAY_PIN_3 11
#define VOLTAGE_DIVIDER_RELAY_PIN_4 10
#define VOLTAGE_DIVIDER_RELAY_PIN_5 9


#define INPUT_PIN_RX1 A0
#define INPUT_PIN_RX2 A6
#define INPUT_PIN_RX3 A3
#define INPUT_PIN_RX4 A2
#define I2C_SCL_PIN A4
#define I2C_SDA_PIN A5
#define INPUT_PIN_RX5 A1
#define CONSTANT_CURRENT_INPUT_PIN A7

#define START_BUTTON_PIN 2
#define SCREEN_BUTTON_PIN 3

// Define constant
#define NUM_OF_SENSOR 9
#define MAX_LINE 9
#define NUM_OF_BUTTONS 2

#define REF_VOL 5.00  //Volt
#define MY_CONSTANT_CURRENT 0.113 // Amp

//#define Rc 10.41  //ohm   Rc=1.25/I; I=0.121A
#define R1 100   //ohm
#define R2 150    //ohm
#define R3 1.512   //Kohm
#define R4 1.512   //Kohm
#define R5 23.6   //Kohm

#define LiquidCrystal_I2C_ADDRESS 0x27

const int PowerPinArray[9] = {VOLTAGE_DIVIDER_POWER_PIN_1, VOLTAGE_DIVIDER_RELAY_PIN_2, VOLTAGE_DIVIDER_RELAY_PIN_3, VOLTAGE_DIVIDER_RELAY_PIN_4, VOLTAGE_DIVIDER_RELAY_PIN_5,
                              CONSTANT_CURRENT_RELAY_PIN_1, CONSTANT_CURRENT_RELAY_PIN_2, CONSTANT_CURRENT_RELAY_PIN_3, CONSTANT_CURRENT_RELAY_PIN_4};
                              
const int SensorInputPinArray[6] = {INPUT_PIN_RX1, INPUT_PIN_RX2, INPUT_PIN_RX3, INPUT_PIN_RX4, INPUT_PIN_RX5, CONSTANT_CURRENT_INPUT_PIN};

const double Rref = REF_VOL/(1023.0*MY_CONSTANT_CURRENT);
const double ConstantResistanceArray[6] = {R1, R2, R3, R4, R5, Rref};

const double MinXaxRes[2][NUM_OF_SENSOR] = {150.0, 200.0, 2.0, 2.0, 40.0, 12.0, 44.0, 44.0, 5.2,
                                           80.0, 100.0, 1.0, 1.0,  6.5,  8.0, 18.0, 18.0, 3.5};
const String SensorName[9] = {"TPS1", "TPS2", "TISS", "TOSS", "OiTSol", "SOL_TCC", "SOL_A", "SOL_B", "OiPSol"};

int VoltageValue[NUM_OF_SENSOR] = {0,0,0,0,0,0,0,0,0};
int TopLine = 0;

// Button Logic
const byte InputPinsArray[NUM_OF_BUTTONS] = {START_BUTTON_PIN, SCREEN_BUTTON_PIN};
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
  
  //analogReference(EXTERNAL);  // Vref connect to 5V pin is 4.94V

  for (int i=0; i<9; i++) {
    pinMode(PowerPinArray[i], OUTPUT); digitalWrite(PowerPinArray[i], HIGH);
  }
  digitalWrite(VOLTAGE_DIVIDER_POWER_PIN_1, LOW);
  
  for(int i=0; i<NUM_OF_BUTTONS; i++)   // set button's pin
    pinMode(InputPinsArray[i], INPUT_PULLUP);
  
  lcd.clear();                      // clear the lcd
  lcd.setCursor(0, 0); lcd.print("Gear Diagnosis"); // column 0 , row 0 i.e. top line of LCD
  lcd.setCursor(0, 1); lcd.print("System V1.2 Made By:");
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
    int reading = digitalRead(InputPinsArray[i]);
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
    lcd.setCursor(0, 0); lcd.print("Wait..... (25 sec)"); // column 0 , row 0
    lcd.setCursor(0,1);  lcd.print("Processing..."); // column 0 , row 1
    
    TopLine = 0;
    unsigned long Time = millis();
    for (int i=0; i<NUM_OF_SENSOR; i++) VoltageValue[i] = CheckValue(i+1, Time);
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

  byte castumChar[8] = {0x00, 0x0E, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x1B};  // ohm
  lcd.createChar(1, castumChar);

  castumChar[0]=0x01; castumChar[1]=0x01; castumChar[2]=0x03; castumChar[3]=0x02; castumChar[4]=0x12; castumChar[5]=0x0A; castumChar[6]=0x0C; castumChar[7]=0x04; // OK
  lcd.createChar(2, castumChar);
  
  castumChar[0]=0x11; castumChar[1]=0x0A; castumChar[2]=0x0A; castumChar[3]=0x04; castumChar[4]=0x04; castumChar[5]=0x0A; castumChar[6]=0x0A; castumChar[7]=0x11; // NOT_OK
  lcd.createChar(3, castumChar);

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
    if (line[i]==2 || line[i]==3 || line[i]==4) lineMsg[i] += "K";
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
  const unsigned long RunTime = 25189;
  double Time = 100.0*(millis()-StartTime)/RunTime;
  unsigned int sensorValue = 0;
  int input_pin_num = 0;
  if (resNum < 6) input_pin_num = SensorInputPinArray[resNum-1];
  else            input_pin_num = SensorInputPinArray[5];
  
  lcd.setCursor(0,2); lcd.print("Checking Sensor "); lcd.print(resNum);
  lcd.setCursor(0,3); lcd.print(Time); lcd.print("% Complete"); 
  ManegeInput(resNum);
  
  for (int i=0; i<30; i++) {
    if (i%2==0) Time = 100.0*(millis()-StartTime)/RunTime;
    lcd.setCursor(0,3); lcd.print(Time); lcd.print("% Complete"); 
    sensorValue += analogRead(input_pin_num);  // variable to store the value coming from the sensor
    delay(50);
  }
  unsigned int AvgVal = sensorValue/30;    // variable to store the value coming from the sensor
  //Serial.print(resNum); Serial.print("; AvgVal: "); Serial.println(AvgVal);
  
  ManegeInput(0); // close all input
  Time = 100.0*(millis()-StartTime)/RunTime;
  lcd.setCursor(0,3); lcd.print(Time); lcd.print("% Complete");
  
  return AvgVal;
}
//***************************************************************************************************************************//
void  ManegeInput(int InputNum) {
  for (int i=1; i<9; i++) digitalWrite(PowerPinArray[i], HIGH);
  digitalWrite(PowerPinArray[0], LOW);
  if     (InputNum == 1)              digitalWrite(PowerPinArray[0], HIGH);
  else if(2<=InputNum && InputNum<=9) digitalWrite(PowerPinArray[InputNum-1], LOW);

  delay(250);
}
//***************************************************************************************************************************//
double CheckResistance(int Value, int resNum) {
  double voltage = REF_VOL*Value/1023.0;
  double Res = 0.0;
  
  if (resNum <= 5) Res = Value*ConstantResistanceArray[resNum-1]/(1024.0-Value);
  else             Res = Value*ConstantResistanceArray[5];

  Serial.print(resNum); Serial.print("; Value: "); Serial.print(Value); Serial.print(", Voltage: "); Serial.print(voltage); Serial.print("[V], Res: "); Serial.print(Res);
  if (resNum <= 5) Serial.println("[Kohm]");
  else             Serial.println("[ohm]");
    
  return Res;
}
//***************************************************************************************************************************//
