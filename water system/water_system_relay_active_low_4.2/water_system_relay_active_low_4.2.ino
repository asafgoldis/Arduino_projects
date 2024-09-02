
//********************************//
//      Water System V4.2
//********************************//

//    IO Connaction:
// A0 - Soil Moisture Unit 1
// A1 - Soil Moisture Unit 2
// A2 - Soil Moisture Unit 3
// A3 - Soil Moisture Unit 4
// A4 - Lcd&RTC - SDA
// A5 - Lcd&RTC - SCL
// A6 - Soil Moisture Unit 5
// A7 - Light Sensor   // 10k resistor
// D0 - NOT CONNECTED 
// D1 - NOT CONNECTED
// D2 - P_S Butten
// D3 - Up Butten
// D4 - Down Butten
// D5 - Enter Butten
// D6 - N_S Butten
// D7 - Relay Unit 1
// D8 - Relay Unit 2
// D9 - Relay Unit 3
// D10 - Relay Unit 4
// D11 - Relay Unit 5
// D12 - power pin
// D13 - NOT CONNECTED
//  Reset butten require pull up resistor of 100K (the reset pin must hold 5V all time)

//********************************//
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
//********************************//
// Define IO
#define SOIL_PIN_1 A0
#define SOIL_PIN_2 A1
#define SOIL_PIN_3 A2
#define SOIL_PIN_4 A3
//#define SDA A4
//#define SCL A5
#define SOIL_PIN_5 A6
#define LIGHT_PIN A7
//#define xxx 0
//#define xxx 1
#define P_S 2
#define Up 3
#define Down 4
#define Enter 5
#define N_S 6
#define RELAY_PIN_1 7
#define RELAY_PIN_2 8
#define RELAY_PIN_3 9
#define RELAY_PIN_4 10
#define RELAY_PIN_5 11
#define POWER_PIN 12
//#define xxx 13
//********************************//
#define MAX_NUM_OF_UNIT 5
#define NUM_OF_BUTTENS 5
//********************************//
#define ROOT_SCREEN 0
#define SETUP_MENU_SCREEN 1
#define SET_PARAMETER_SCREEN 2
#define SET_UNIT_SCREEN 3
#define SET_TIME_SCREEN 4
//********************************//
#define RELAY_ON LOW
#define RELAY_OFF HIGH
//********************************//
#define MINUTE_TO_CHECK 15    // 15 min
#define POWER_PIN_HIGH 5      // 5 sec
#define DELTA 2               // +-2% 
//********************************//
#define TIME_TO_OFF_DISPLAY 10  // 10 sec
//********************************//
#define Cal_Soil_Sensor 935
//********************************//
#define DS3231_I2C_ADDRESS 0x68
#define LiquidCrystal_I2C_ADDRESS 0x3F
//*********************************************************************************************//

const int SOIL_PIN[MAX_NUM_OF_UNIT] = {SOIL_PIN_1, SOIL_PIN_2, SOIL_PIN_3, SOIL_PIN_4, SOIL_PIN_5};
const byte RELAY_PIN[MAX_NUM_OF_UNIT] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5};

//Input & Button Logic
const byte inputPins[NUM_OF_BUTTENS] = {Enter, Up, Down, N_S, P_S};
byte inputState[NUM_OF_BUTTENS];
byte lastInputState[NUM_OF_BUTTENS] = {HIGH,HIGH,HIGH,HIGH,HIGH};
bool inputFlags[NUM_OF_BUTTENS] = {LOW,LOW,LOW,LOW,LOW};
unsigned long lastDebounceTime[NUM_OF_BUTTENS] = {0,0,0,0,0};
const unsigned long debounceDelay = 5;

//  Manege time
unsigned long last_time_change = 0;
unsigned long last_num;
unsigned long sec_Counter = 0;
unsigned int counter = 0;

//LCD Menu Logic
byte CurrentScreen = 0;
int CurrentSubScreen = 0;
int unit_num = 1;
int num_of_unit = 0;
byte Screen_delta[5] = {4,4,4,1,7};

// STORE DATA
int set_time_screen_parameters[7];
double light_meas;
double soil_meas[MAX_NUM_OF_UNIT];
//  unit 1~5                       soil,light,min time between water,max water time
int praneters[MAX_NUM_OF_UNIT][4] = { 60, 85, 180, 30,
                                      60, 85, 180, 30,
                                      60, 85, 180, 30,
                                      60, 85, 180, 30,
                                      60, 85, 180, 30};
// unit 1~5                             day,month,year,HH, MM, control
unsigned int water[MAX_NUM_OF_UNIT][6] = {31, 12, 11, 12, 30, RELAY_OFF,
                                          31, 12, 12, 12, 30, RELAY_OFF,
                                          31, 12, 13, 12, 30, RELAY_OFF,
                                          31, 12, 14, 12, 30, RELAY_OFF,
                                          31, 12, 15, 12, 30, RELAY_OFF};
// unit 1~5          water start, water stop
unsigned long water_SS[MAX_NUM_OF_UNIT][2];

//*********************************************************************************************//
LiquidCrystal_I2C lcd(LiquidCrystal_I2C_ADDRESS,2,1,0,4,5,6,7);  // Set the LCD I2C address
//*********************************************************************************************//

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val) {
  return( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val) {
  return( (val/16*10) + (val%16) );
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
//*********************************************************************************************//

void setup(){
  Wire.begin();
  //Serial.begin(9600);
  //Serial.println(""); Serial.println("Water System by Asaf Goldis"); Serial.println("");
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(30,59,23,4,22,12,17);

  byte Tipa[8] = {0b00100,
                  0b00100,
                  0b01110,
                  0b01110,
                  0b11111,
                  0b11111,
                  0b11111,
                  0b01110 };
  
  // set up the LCD's number of columns and rows:
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.createChar(1, Tipa);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  
  lcd.setCursor(0,0);
  lcd.write(1); lcd.print(" Water System "); lcd.write(1);
  lcd.setCursor(1,1); lcd.print("By Asaf Goldis");
  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); // retrieve data from DS3231
  set_time_screen_parameters[0] = second;
  set_time_screen_parameters[1] = minute;
  set_time_screen_parameters[2] = hour;
  set_time_screen_parameters[3] = dayOfWeek;
  set_time_screen_parameters[4] = dayOfMonth;
  set_time_screen_parameters[5] = month;
  set_time_screen_parameters[6] = year;
  
  pinMode(POWER_PIN, OUTPUT);
  for(int i=0; i<NUM_OF_BUTTENS; i++)   // set butten's pin
    pinMode(inputPins[i], INPUT_PULLUP);
  
  for(int i=0; i<MAX_NUM_OF_UNIT; i++)  // set relay's pin
    pinMode(RELAY_PIN[i], OUTPUT);
  
  Turn_Off_Anused_Relay();
  num_of_unit = 1;
  digitalWrite(POWER_PIN,HIGH); delay(1000);
  
  Meas_Light();   //read light sensor
  Meas_Soil();    //read soil sensor's
  
  delay(1000);
  lcd.clear();
  digitalWrite(POWER_PIN,LOW);
}   // setup

void loop() {
  unsigned long currentSecond = millis()/1000;
  counter++;
  Auto_off_display();
  setInputFlags();
  resolveInputFlags();
  Turn_Off_Anused_Relay();
  
  if ( water[0][5]==RELAY_ON || water[1][5]==RELAY_ON || water[2][5]==RELAY_ON || water[3][5]==RELAY_ON || water[4][5]==RELAY_ON || (currentSecond%(60*MINUTE_TO_CHECK)==0) ) {      // 60*5 = 5 min
    if (!digitalRead(POWER_PIN)) {
      digitalWrite(POWER_PIN,HIGH);
      delay(500);
      last_num = currentSecond;
    }
    sec_Counter = currentSecond;
    digitalWrite(POWER_PIN,HIGH);
  }
  else if(last_num+POWER_PIN_HIGH < currentSecond)
    digitalWrite(POWER_PIN,LOW);
  
  if (counter%15 == 0) {
    Meas_Light();
  }
  
  if (digitalRead(POWER_PIN)) {
    Meas_Soil();
    Manege_Relay(currentSecond);
  }
}   // end loop

//*********************************************************************************************//
void Auto_off_display() {
  if (millis()-last_time_change > 1000*TIME_TO_OFF_DISPLAY)
    lcd.setBacklight(LOW);
  else
    lcd.setBacklight(HIGH);
}

void Turn_Off_Anused_Relay() {                     // turn off an used relay's
  for(int i=num_of_unit; i<MAX_NUM_OF_UNIT; i++) {
    digitalWrite(RELAY_PIN[i], RELAY_OFF);
    water[i][5] = RELAY_OFF;
  }
}

void Meas_Light() {                                //read light sensor
  light_meas = analogRead(LIGHT_PIN)*100.0/1024;
}

void Meas_Soil() {                                 //read soil sensor's
  for (int j=0; j<MAX_NUM_OF_UNIT; j++)
    soil_meas[j]=(Cal_Soil_Sensor-analogRead(SOIL_PIN[j]))*100.0/Cal_Soil_Sensor;
}

void Manege_Relay(unsigned long currentSecond) {
int delta;
  for (int i=0; i<num_of_unit; i++){
    if (water[i][5]==RELAY_OFF) delta = -DELTA;                     //histerzis
    else                        delta = DELTA;
    
    if (soil_meas[i] < (praneters[i][0]+delta) &&
          light_meas < (praneters[i][1]+delta) &&
       (currentSecond-water_SS[i][1]) >= 60*praneters[i][2]) {
      if (water[i][5]==RELAY_OFF) water_SS[i][0] = currentSecond;
      if (currentSecond-water_SS[i][0] <= praneters[i][3]) {
        digitalWrite(RELAY_PIN[i],RELAY_ON);     // start water
        water[i][5] = RELAY_ON; 
      }
      else {
        water_SS[i][1] = currentSecond;
        update_last_water(i);
        digitalWrite(RELAY_PIN[i],RELAY_OFF);
        water[i][5] = RELAY_OFF;
      }
    }
    else {
      if (water[i][5]==RELAY_ON) {
        water_SS[i][1] = currentSecond;
        update_last_water(i);
      }
      digitalWrite(RELAY_PIN[i],RELAY_OFF);
      water[i][5] = RELAY_OFF;
    }
  }
}

void setInputFlags() {
  for(int i=0; i<NUM_OF_BUTTENS; i++) {
    int reading = digitalRead(inputPins[i]);
    if (reading != lastInputState[i]) {
      lastDebounceTime[i] = millis();
    }
    if ((millis()-lastDebounceTime[i]) > debounceDelay) {
      if (reading != inputState[i]) {
        inputState[i] = reading;
        if (inputState[i] == LOW) inputFlags[i] = HIGH;
      }
    }
    lastInputState[i] = reading;
  }
}

void resolveInputFlags() {
  bool change = false;
  for(int i=0; i<NUM_OF_BUTTENS; i++) {
    if(inputFlags[i] == HIGH) {
      change = true;
      last_time_change = millis();
      inputAction(i);
      inputFlags[i] = LOW;
      printScreen();
    }
  }
  if (change == false && ROOT_SCREEN == 0)
    printScreen();
}

void inputAction(int input) {
  lcd.clear();
  if      (CurrentScreen == ROOT_SCREEN)          Root_Screen_Action(input);
  else if (CurrentScreen == SETUP_MENU_SCREEN)    Setup_Menu_Screen_Action(input);
  else if (CurrentScreen == SET_PARAMETER_SCREEN) Set_Parameter_Screen_Action(input);
  else if (CurrentScreen == SET_UNIT_SCREEN)      Set_Unit_Screen_Action(input);
  else if (CurrentScreen == SET_TIME_SCREEN)      Set_Time_Screen_Action(input);
}

void Root_Screen_Action(int input) {
  if(input == 0) {                                                              // Enter
      CurrentScreen = SETUP_MENU_SCREEN;
      CurrentSubScreen = 0;
    }
    else if(input == 1) {                                                      // Up
      if (unit_num == num_of_unit) unit_num = 1;
      else                         unit_num++;
    }
    else if(input == 2) {                                                      // Down
      if (unit_num == 1) unit_num = num_of_unit;
      else               unit_num--;
    }
    else if(input == 3) next_Screen();                                          // N_S
    else if(input == 4) previous_Screen();                                      // P_S
}

void Setup_Menu_Screen_Action(int input) {
  String setup_screen_parameters;
  if(input == 0) {                                                              // Enter
      if      (CurrentSubScreen == 0) CurrentScreen = SET_PARAMETER_SCREEN;     // Set Parameter
      else if (CurrentSubScreen == 1) CurrentScreen = SET_UNIT_SCREEN;          // Set num of unit
      else if (CurrentSubScreen == 2) CurrentScreen = SET_TIME_SCREEN;          // Set Time
      else if (CurrentSubScreen == 3) CurrentScreen = ROOT_SCREEN;              // Go Back
      CurrentSubScreen = 0;
    }
    else if(input == 1) {                                                       // Up
      if (CurrentSubScreen == 0) {     // Set Parameter
        if (unit_num == num_of_unit) unit_num = 1;
        else                         unit_num++;
        setup_screen_parameters = "U" + String(unit_num);
      }
    }
    else if(input == 2) {                                                        // Down
      if (CurrentSubScreen == 0) {     // Set Parameter
        if (unit_num == 1) unit_num = num_of_unit;
        else               unit_num--;
        setup_screen_parameters = "U" + String(unit_num);
      }
    }
    else if(input == 3) next_Screen();                                            // N_S
    else if(input == 4) previous_Screen();                                        // P_S
}

void Set_Parameter_Screen_Action(int input) {
  if(input == 0) {                                                                // Enter
    CurrentScreen = SETUP_MENU_SCREEN;
    CurrentSubScreen = 0;
  }
  else if(input == 1) praneters[unit_num-1][CurrentSubScreen]++;                  // Up
  else if(input == 2) praneters[unit_num-1][CurrentSubScreen]--;                  // Down
  else if(input == 3) next_Screen();                                              // N_S
  else if(input == 4) previous_Screen();                                          // P_S
  fix_praneters();
}

void Set_Unit_Screen_Action(int input) {
  int MAX = MAX_NUM_OF_UNIT;
  int MIN = 1;
  
  if(input == 0) {                                                                 // Enter
    CurrentScreen = SETUP_MENU_SCREEN;
    CurrentSubScreen = 0;
  }
  else if(input == 1) num_of_unit++;                                                // Up
  else if(input == 2) num_of_unit--;                                                // Down
  
  if (num_of_unit > MAX) num_of_unit = MIN;
  if (num_of_unit < MIN) num_of_unit = MAX;
}

void Set_Time_Screen_Action(int input) {
  if(input == 0) {                                                                  // Enter
    setDS3231time(set_time_screen_parameters[0], set_time_screen_parameters[1],
                  set_time_screen_parameters[2], set_time_screen_parameters[3],
                  set_time_screen_parameters[4], set_time_screen_parameters[5],
                  set_time_screen_parameters[6]);
    CurrentScreen = SETUP_MENU_SCREEN;
    CurrentSubScreen = 0;
  }
  else if(input == 1) set_time_screen_parameters[CurrentSubScreen]++;               // Up
  else if(input == 2) set_time_screen_parameters[CurrentSubScreen]--;               // Down
  else if(input == 3) next_Screen();                                                // N_S
  else if(input == 4) previous_Screen();                                            // P_S
  fix_time();
}

void fix_praneters() {
  int MAX, MIN;
  for (int i=0; i<MAX_NUM_OF_UNIT; i++) {
    for (int j=0; j<4; j++) {
      switch(j){
      case 0: MIN = 0; MAX = 100; break;    // soil moisture %
      case 1: MIN = 0; MAX = 100; break;    // light %
      case 2: MIN = 0; MAX = 720; break;    // min time between water (min)
      case 3: MIN = 1; MAX = 240; break;    // max water time (sec)
      }
      if (praneters[i][j] > MAX) praneters[i][j] = MIN;
      if (praneters[i][j] < MIN) praneters[i][j] = MAX;
    }
  }
}

void fix_time() {
  int MAX, MIN;
  for (int i=0; i<7; i++) {
    switch(i){
    case 0: MIN = 0; MAX = 59; break;   // sec
    case 1: MIN = 0; MAX = 59; break;   // min
    case 2: MIN = 0; MAX = 23; break;   // hour
    case 3: MIN = 1; MAX = 7;  break;   // day name
    case 4: MIN = 1; MAX = 31; break;   // date
    case 5: MIN = 1; MAX = 12; break;   // month
    case 6: MIN = 0; MAX = 99; break;   // year
    }
    if (set_time_screen_parameters[i] > MAX) set_time_screen_parameters[i] = MIN;
    if (set_time_screen_parameters[i] < MIN) set_time_screen_parameters[i] = MAX;
  }
}

void next_Screen(){
  byte numOfScreens = Screen_delta[CurrentScreen];
  if (CurrentSubScreen == numOfScreens-1) CurrentSubScreen = 0;
  else                                    CurrentSubScreen++;
}

void previous_Screen(){
  byte numOfScreens = Screen_delta[CurrentScreen];
  if (CurrentSubScreen == 0) CurrentSubScreen = numOfScreens-1;
  else                       CurrentSubScreen--;
}

void printScreen() {
  byte back[8] = {0b00100,
                  0b01100,
                  0b11111,
                  0b01101,
                  0b00101,
                  0b00001,
                  0b00001,
                  0b11111 };
  lcd.createChar(2, back);
  
  lcd.setCursor(0,0);
  if (CurrentScreen == ROOT_SCREEN) {
    String root_screen[4][2] = { {""                                  , ""   },
                                 {"Light"                             , "%  "},
                                 {"Last Water U" + String(unit_num)   , ""   },
                                 {"Soil Moisture U" + String(unit_num), "%"  } };
    correct_date_time(&root_screen[0][0], &root_screen[0][1]);
    
    unsigned long num = 60*MINUTE_TO_CHECK + sec_Counter - millis()/1000;
    int MM = num/60;
    int SS = num - MM*60;
    root_screen[3][1] = "%       ";
    if      (soil_meas[unit_num-1] < 10)   root_screen[3][1] += "  ";
    else if (soil_meas[unit_num-1] != 100) root_screen[3][1] += " ";
    if (MM < 10) root_screen[3][1] += " ";
    root_screen[3][1] += String(MM); root_screen[3][1] += ":"; 
    if (SS < 10) {root_screen[3][1] += "0";} root_screen[3][1] += String(SS);
    
    lcd.print(root_screen[CurrentSubScreen][0]);
    lcd.setCursor(0,1);
    switch(CurrentSubScreen){
    case 0: lcd.print(""); break;
    case 1: lcd.print(String(int(light_meas))); break;
    case 2: if (water[unit_num-1][5] == RELAY_OFF) lcd.print(lastWaterAsString());
            else                                   lcd.print("NOW!           ");
      break;
    case 3: lcd.print(String(int(soil_meas[unit_num-1]))); break;
    }
    lcd.print(root_screen[CurrentSubScreen][1]);
  }
  else if (CurrentScreen == SETUP_MENU_SCREEN) {                                         // setup menu screen
    String setup_menu_screen[4][2] = { {"Setup Menu", "Set Parameter"   },
                                       {"Setup Menu", "Set Unit Number" },
                                       {"Setup Menu", "Set Time"        },
                                       {"Setup Menu", "Go Back"         } };
    
    lcd.print(setup_menu_screen[CurrentSubScreen][0]);
    lcd.setCursor(0,1);
    lcd.print(setup_menu_screen[CurrentSubScreen][1]); lcd.print(" ");
    if (CurrentSubScreen == 0) {lcd.print("U"); lcd.print(unit_num);}
    else if (CurrentSubScreen == 3) lcd.write(2);
  }
  else if (CurrentScreen == SET_PARAMETER_SCREEN) {                                         // set parameter screen
    String set_parameter_screen[4][3] = { {"Soil Moisture"   , "U"      , "%"   },
                                          {"Light"           , "U"      , "%"   },
                                          {"Min Time Between", "Water U", "min" },
                                          {"Max Water Time"  , "U"      , " sec"} };
                                        
    lcd.print(set_parameter_screen[CurrentSubScreen][0]); 
    lcd.setCursor(0,1);
    lcd.print(set_parameter_screen[CurrentSubScreen][1]);  lcd.print(unit_num); lcd.print(": ");
    lcd.print(praneters[unit_num-1][CurrentSubScreen]);
    lcd.print(set_parameter_screen[CurrentSubScreen][2]);
  }
  else if (CurrentScreen == SET_UNIT_SCREEN) {                                         // set num of unit screen
    lcd.print("Set Num Of Unit"); 
    lcd.setCursor(0,1);
    lcd.print(num_of_unit);  lcd.print(" Units");
  }
  else if (CurrentScreen == SET_TIME_SCREEN) {                                         // set time screen
    String set_time_screen[7][2] = { {"Set Date & Time:", " sec"     },
                                     {"Set Date & Time:", " min"     },
                                     {"Set Date & Time:", " hour"    },
                                     {"Set Date & Time:", " day name"},
                                     {"Set Date & Time:", " date"    },
                                     {"Set Date & Time:", " month"   },
                                     {"Set Date & Time:", " year"    } };
                                   
    lcd.print(set_time_screen[CurrentSubScreen][0]); lcd.print(unit_num);
    lcd.setCursor(0,1);
    if (CurrentSubScreen == 3) {
      String day_name;
      switch(set_time_screen_parameters[CurrentSubScreen]) {
      case 1: day_name = "Sun."; break;
      case 2: day_name = "Mon."; break;
      case 3: day_name = "Tue."; break;
      case 4: day_name = "Wed."; break;
      case 5: day_name = "Thu."; break;
      case 6: day_name = "Fri."; break;
      case 7: day_name = "Sat."; break;
      }
      lcd.print(day_name);
    }
    else {
      if (set_time_screen_parameters[CurrentSubScreen] < 10) lcd.print("0");
      lcd.print(set_time_screen_parameters[CurrentSubScreen]);
    }
    lcd.print(set_time_screen[CurrentSubScreen][1]);
  }
}   // print screen

void correct_date_time(String *correct_date, String *correct_time) {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
  *correct_date = "";
  *correct_time = "    ";
  
  switch(dayOfWeek){
  case 1: *correct_date += "Sun."; break;
  case 2: *correct_date += "Mon."; break;
  case 3: *correct_date += "Tue."; break;
  case 4: *correct_date += "Wed."; break; //lcd.print("Wednesday"); break;
  case 5: *correct_date += "Thu."; break;
  case 6: *correct_date += "Fri."; break;
  case 7: *correct_date += "Sat."; break;
  }
  
  *correct_date += "  "; *correct_date += String(dayOfMonth, DEC);
  *correct_date += "/";  *correct_date += String(month, DEC);
  *correct_date += "/";  *correct_date += String(year, DEC);
  
  if (hour<10  ) {*correct_time += "0";} *correct_time += String(hour  , DEC); *correct_time += ":";
  if (minute<10) {*correct_time += "0";} *correct_time += String(minute, DEC); *correct_time += ":";
  if (second<10) {*correct_time += "0";} *correct_time += String(second, DEC);
}

String lastWaterAsString() {
  String lastWater = "";
  lastWater += String(water[unit_num-1][0]); lastWater += "/";
  lastWater += String(water[unit_num-1][1]); lastWater += "/";
  lastWater += String(water[unit_num-1][2]); lastWater += ",";
  if (water[unit_num-1][3] < 10) lastWater += "0";
  lastWater += String(water[unit_num-1][3]); lastWater += ":";
  if (water[unit_num-1][4] < 10) lastWater += "0";
  lastWater += String(water[unit_num-1][4]);
  return (lastWater);
}

void update_last_water(int unit) {  
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year); // retrieve data from DS3231
  water[unit][0] = dayOfMonth;
  water[unit][1] = month;
  water[unit][2] = year;
  water[unit][3] = hour;
  water[unit][4] = minute;
}

