//הפעלה כך שלא יהיה מתח קבוע על חיישני הלחות כדי שלא יחלידו

/* connaction:
 *  A0 - Soil Moisture Unit 1
 *  A1 - Soil Moisture Unit 2
 *  A2 - Soil Moisture Unit 3
 *  A3 - Soil Moisture Unit 4
 *  A4 - Lcd&RTC - SDA
 *  A5 - Lcd&RTC - SCL
 *  A6 - Soil Moisture Unit 5
 *  A7 - Light Sensor
 *  D0 - NOT CONNECTED 
 *  D1 - NOT CONNECTED
 *  D2 - Enter Butten
 *  D3 - Up Butten
 *  D4 - Down Butten
 *  D5 - N_S Butten
 *  D6 - P_S Butten
 *  D7 - Relay Unit 1
 *  D8 - Relay Unit 2
 *  D9 - Relay Unit 3
 *  D10 - Relay Unit 4
 *  D11 - Relay Unit 5
 *  D12 - power pin
 *  D13 - NOT CONNECTED
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <stdio.h>

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
#define Enter 2
#define Up 3
#define Down 4
#define N_S 5
#define P_S 6
#define RELAY_PIN_1 7
#define RELAY_PIN_2 8
#define RELAY_PIN_3 9
#define RELAY_PIN_4 10
#define RELAY_PIN_5 11
#define POWER_PIN 12
//#define xxx 13

#define MAX_NUM_OF_UNIT 5
#define NUM_OF_BUTTENS 5
#define MINUTE_TO_CHECK 1

#define DS3231_I2C_ADDRESS 0x68
#define LiquidCrystal_I2C_ADDRESS 0x27

//*********************************************************************************************//

const int SOIL_PIN[MAX_NUM_OF_UNIT]={SOIL_PIN_1, SOIL_PIN_2, SOIL_PIN_3, SOIL_PIN_4, SOIL_PIN_5};
const byte RELAY_PIN[MAX_NUM_OF_UNIT]={RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5};

//Input & Button Logic
const byte inputPins[NUM_OF_BUTTENS] = {Enter, Up, Down, N_S, P_S};
byte inputState[NUM_OF_BUTTENS];
//byte lastInputState[NUM_OF_BUTTENS] = {LOW,LOW,LOW,LOW,LOW};
byte lastInputState[NUM_OF_BUTTENS] = {HIGH,HIGH,HIGH,HIGH,HIGH};
bool inputFlags[NUM_OF_BUTTENS] = {LOW,LOW,LOW,LOW,LOW};
//bool inputFlags[NUM_OF_BUTTENS] = {HIGH,HIGH,HIGH,HIGH,HIGH};
unsigned long lastDebounceTime[NUM_OF_BUTTENS] = {0,0,0,0,0};
unsigned long debounceDelay = 5;
unsigned long last_time_change = 0;

//LCD Menu Logic
byte print_mode = 0;
byte currentScreen = 0;
byte unit_num = 1;
byte num_of_unit = 1;
byte Screen_delta[5] = {4,4,4,1,7};

// STORE DATA
byte set_time_screen_parameters[7];

double light_meas;
double soil_meas[MAX_NUM_OF_UNIT];

//       soil, light, min time between water, max water time
//unit1 |     |     |                       |
//unit2 |     |     |                       |
//unit3 |     |     |                       |
//unit4 |     |     |                       |
//unit5 |     |     |                       |
byte praneters[MAX_NUM_OF_UNIT][4] = {75, 75, 5, 30,
                                      75, 75, 5, 30,
                                      75, 75, 5, 30,
                                      75, 75, 5, 30,
                                      75, 75, 5, 30};

//       day, month, year, HH,   MM,  control
//unit1 |    |      |     |     |    |
//unit2 |    |      |     |     |    |
//unit3 |    |      |     |     |    |
//unit4 |    |      |     |     |    |
//unit5 |    |      |     |     |    |
unsigned int water[MAX_NUM_OF_UNIT][6] = {31, 12, 2011, 12, 30, HIGH,
                                          31, 12, 2012, 12, 30, HIGH,
                                          31, 12, 2013, 12, 30, HIGH,
                                          31, 12, 2014, 12, 30, HIGH,
                                          31, 12, 2015, 12, 30, HIGH};

//       water start, water stop
//unit1 |           |
//unit2 |           |
//unit3 |           |
//unit4 |           |
//unit5 |           |
unsigned long water_SS[MAX_NUM_OF_UNIT][2];

//unsigned long currentSecond;
unsigned long last_num;
unsigned long sec_Counter = 0;

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

void setDS3231time( byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
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
  //Serial.println("");
  //Serial.println("Water System by Asaf Goldis");
  //Serial.println("");
  
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS3231time(30,59,23,4,22,12,17);

  byte Tipa[8] = {
    0b00100,
    0b00100,
    0b01110,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b01110
  };
  
  // set up the LCD's number of columns and rows:
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.createChar(1, Tipa);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  
  lcd.setCursor(0,0);
  lcd.write(1); lcd.print(" Water System "); lcd.write(1);
  lcd.setCursor(1,1); lcd.print("By Asaf Goldis");

  readDS3231time( &set_time_screen_parameters[0], &set_time_screen_parameters[1],
                  &set_time_screen_parameters[2], &set_time_screen_parameters[3],
                  &set_time_screen_parameters[4], &set_time_screen_parameters[5],
                  &set_time_screen_parameters[6]); // retrieve data from DS3231
  
  for(int i=0; i<NUM_OF_BUTTENS; i++) 
    pinMode(inputPins[i], INPUT_PULLUP);
  
  for(int i=0; i<MAX_NUM_OF_UNIT; i++) {
    pinMode(RELAY_PIN[i], OUTPUT);
    digitalWrite (RELAY_PIN[i],HIGH);             // turn all relay's off
    water[i][5] = HIGH;
  }
  
  print_mode = 0;
  delay(2000); lcd.clear();
}   // setup

void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentSecond = currentMillis/1000;
  //unsigned long currentSecond = currentMillis/1000;
  Auto_off_display();
  
  if (currentSecond%(60*MINUTE_TO_CHECK) == 0)
    last_num = currentSecond;

  if (digitalRead(POWER_PIN)) digitalWrite(13,HIGH);
  else digitalWrite(13,LOW);
  
  if ( (water[0][5] & water[1][5] & water[2][5] & water[3][5] & water[4][5] == LOW) || (currentSecond%(60*MINUTE_TO_CHECK) == 0) ) {      // 60*5 = 5 min
    if (!digitalRead(POWER_PIN)) {
      digitalWrite(POWER_PIN,HIGH);
      delay(500);
      sec_Counter = currentSecond;
    }
    digitalWrite(POWER_PIN,HIGH);
  }
  else if(sec_Counter+10 < currentSecond)
    digitalWrite(POWER_PIN,LOW);
  
  if (digitalRead(POWER_PIN)) {
    for (int j=0; j<MAX_NUM_OF_UNIT; j++)                             //read soil sensor's
      soil_meas[j]=(1023-analogRead(SOIL_PIN[j]))*100.0/1024;
  }

  if (currentSecond%2 == 0)
    light_meas = analogRead(LIGHT_PIN)*100.0/1024;                 //read light sensor
    
  setInputFlags();
  resolveInputFlags();

  for(int i=num_of_unit; i<MAX_NUM_OF_UNIT; i++) {              // turn off an used relay's
    digitalWrite (RELAY_PIN[i],HIGH);
    water[i][5] = HIGH;
  }
  
  int delta;
  for (int j=0; j<MAX_NUM_OF_UNIT; j++){
    if (water[j][5] == HIGH) delta = -3;                            //histerzis
    else                     delta = 3;
    
    if (soil_meas[j]<(praneters[j][0] + delta) &&
        light_meas < (praneters[j][1] + delta) &&
        (currentSecond - water_SS[j][1]) >= 60*praneters[j][2]) {  //unit 1+2+3+4+5
      
      if (water[j][5]==HIGH) water_SS[j][0] = currentSecond;
      if (currentSecond - water_SS[j][0] <= praneters[j][3]){
        digitalWrite(RELAY_PIN[j],LOW);     // start water
        water[j][5]=LOW; 
      }
      else {
        water_SS[j][1] = currentSecond;
        update_last_water(j);
        digitalWrite (RELAY_PIN[j],HIGH);
        water[j][5] = HIGH;
      }
    }
    else {
      if (water[j][5]==LOW) {
        water_SS[j][1] = currentSecond;
        update_last_water(j);
      }
      digitalWrite (RELAY_PIN[j],HIGH);
      water[j][5] = HIGH;
    }
  }

}   // end loop

void Auto_off_display() {
  if (millis()-last_time_change > 10000) lcd.setBacklight(LOW);
  else                                     lcd.setBacklight(HIGH);
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
  if (change == false && print_mode == 0)
    printScreen();
}

void inputAction(int input) {
  lcd.clear();
  if      (print_mode == 0) General_Screen_Action(input);
  else if (print_mode == 1) Setup_Screen_Action(input);
  else if (print_mode == 2) Set_Parameter_Screen_Action(input);
  else if (print_mode == 3) Set_Num_Of_Unit_Screen_Action(input);
  else if (print_mode == 4) Set_Time_Screen_Action(input);
}

void General_Screen_Action(int input) {
  if(input == 0) {                                      // Enter
      print_mode = 1;             // setup menu
      currentScreen = 0;
    }
    else if(input == 1) {                               // Up
      if (unit_num == num_of_unit) unit_num = 1;
      else                         unit_num++;
    }
    else if(input == 2) {                              // Down
      if (unit_num == 1) unit_num = num_of_unit;
      else               unit_num--;
    }
    else if(input == 3) next_Screen();                  // N_S
    else if(input == 4) previous_Screen();                   // P_S
}

void Setup_Screen_Action(int input) {
  String setup_screen_parameters;
  if(input == 0) {                                                              // Enter
      if      (currentScreen == 0) print_mode = 2;      // Set Parameter
      else if (currentScreen == 1) print_mode = 3;      // Set num of unit
      else if (currentScreen == 2) print_mode = 4;      // Set Time
      else if (currentScreen == 3) print_mode = 0;      // Go Back
      currentScreen = 0;
    }
    else if(input == 1) {                                                         // Up
      if (currentScreen == 0) {     // Set Parameter
        if (unit_num == num_of_unit) unit_num = 1;
        else                         unit_num++;
        setup_screen_parameters = "U" + String(unit_num);
      }
    }
    else if(input == 2) {                                                         // Down
      if (currentScreen == 0) {     // Set Parameter
        if (unit_num == 1) unit_num = num_of_unit;
        else               unit_num--;
        setup_screen_parameters = "U" + String(unit_num);
      }
    }
    else if(input == 3) next_Screen();                                            // N_S
    else if(input == 4) previous_Screen();                                             // P_S
}

void Set_Parameter_Screen_Action(int input) {
  if(input == 0) {                                                          // Enter
    print_mode = 1;             // setup menu
    currentScreen = 0;
  }
  else if(input == 1) praneters[unit_num-1][currentScreen]++;            // Up
  else if(input == 2) praneters[unit_num-1][currentScreen]--;            // Down
  else if(input == 3) next_Screen();                                     // N_S
  else if(input == 4) previous_Screen();                                 // P_S
  fix_praneters();
}

void Set_Num_Of_Unit_Screen_Action(int input) {
  int MAX = MAX_NUM_OF_UNIT;
  int MIN = 1;
  
  if(input == 0) {                                                          // Enter
    print_mode = 1;             // setup menu
    currentScreen = 0;
  }
  else if(input == 1) num_of_unit++;                                        // Up
  else if(input == 2) num_of_unit--;                                        // Down
  
  if (num_of_unit > MAX) num_of_unit = MIN;
  if (num_of_unit < MIN) num_of_unit = MAX;
}

void Set_Time_Screen_Action(int input) {
  /*
  byte second = set_time_screen_parameters[0];
  byte minute = set_time_screen_parameters[1];
  byte hour = set_time_screen_parameters[2];
  byte dayOfWeek = set_time_screen_parameters[3];
  byte dayOfMonth = set_time_screen_parameters[4];
  byte month = set_time_screen_parameters[5];
  byte year = set_time_screen_parameters[6];
    */
  if(input == 0) {                                                                  // Enter
    //setDS3231time(second, minute, hour, dayOfWeek, dayOfMonth, month, year);
    setDS3231time(set_time_screen_parameters[0], set_time_screen_parameters[1],
                  set_time_screen_parameters[2], set_time_screen_parameters[3],
                  set_time_screen_parameters[4], set_time_screen_parameters[5],
                  set_time_screen_parameters[6]);
    print_mode = 1;             // setup menu
    currentScreen = 0;
  }
  else if(input == 1) set_time_screen_parameters[currentScreen]++;                  // Up
  else if(input == 2) set_time_screen_parameters[currentScreen]--;                  // Down
  else if(input == 3) next_Screen();                                                // N_S
  else if(input == 4) previous_Screen();                                            // P_S
  fix_time();
}

void fix_praneters() {
  int MAX, MIN;
  for (int i=0; i<MAX_NUM_OF_UNIT; i++) {
    for (int j=0; j<4; j++) {
      switch(j){
      case 0: MIN = 0; MAX = 100; break;
      case 1: MIN = 0; MAX = 100; break;
      case 2: MIN = 0; MAX = 120; break;
      case 3: MIN = 1; MAX = 120; break;
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
    case 3: MIN = 1; MAX = 7; break;   // day name
    case 4: MIN = 1; MAX = 31; break;   // date
    case 5: MIN = 1; MAX = 12; break;   // month
    case 6: MIN = 1; MAX = 99; break;   // year
    }
    if (set_time_screen_parameters[i] > MAX) set_time_screen_parameters[i] = MIN;
    if (set_time_screen_parameters[i] < MIN) set_time_screen_parameters[i] = MAX;
  }
}

void next_Screen(){
  byte numOfScreens = Screen_delta[print_mode];
  if (currentScreen == numOfScreens-1) currentScreen = 0;
  else                                 currentScreen++;
}

void previous_Screen(){
  byte numOfScreens = Screen_delta[print_mode];
  if (currentScreen == 0) currentScreen = numOfScreens-1;
  else                    currentScreen--;
}

void printScreen() {
  String general_screen[4][2] = { {""               , "" },                     // print_mode = 0
                                  {"Light"          , "%"},
                                  {"Last Water U"   , "" },
                                  {"Soil Moisture U", "%"} };
  
  String setup_screen[4][2] = { {"Setup Menu", "Set Parameter"},
                                {"Setup Menu", "Set Unit Num" },
                                {"Setup Menu", "Set Time"     },
                                {"Setup Menu", "Go Back"      } };

  String set_parameter_screen[4][3] = { {"Soil Moisture"   , "U"      , " %"},
                                        {"Light"           , "U"      , " %"},
                                        {"Min Time Between", "Water U", " min"},
                                        {"Max Water Time"  , "U"      , " sec"} };
  
  String set_time_screen[7][2] = { {"Set Date & Time:", " sec"},
                                   {"Set Date & Time:", " min"},
                                   {"Set Date & Time:", " hour"},
                                   {"Set Date & Time:", " day name"},
                                   {"Set Date & Time:", " date"},
                                   {"Set Date & Time:", " month"},
                                   {"Set Date & Time:", " year"} };
  
  lcd.setCursor(0,0);
  if (print_mode == 0) {                                         // general screen
    correct_date_time(&general_screen[0][0], &general_screen[0][1]);
    general_screen[2][0] = "Last Water U" + String(unit_num);
    general_screen[3][0] = "Soil Moisture U" + String(unit_num);
    
    unsigned long num = 60*MINUTE_TO_CHECK + last_num - millis()/1000;
    int MM = num/60;
    int SS = num - MM*60;
    general_screen[3][1] = "%        " + String(MM); general_screen[3][1] += ":"; 
    if (SS < 10) {general_screen[3][1] += "0";} general_screen[3][1] += String(SS);
    
    lcd.print(general_screen[currentScreen][0]);
    lcd.setCursor(0,1);
    switch(currentScreen){
    case 0: lcd.print(""); break;
    case 1: lcd.print(String(int(light_meas))); break;
    case 2: if (water[unit_num-1][5]) lcd.print(lastWaterAsString());
            else                      lcd.print("NOW");
      break;
    case 3: lcd.print(String(int(soil_meas[unit_num-1]))); break;
    }
    lcd.print(general_screen[currentScreen][1]);
  }
  else if (print_mode == 1) {                                         // setup menu screen
    lcd.print(setup_screen[currentScreen][0]);
    lcd.setCursor(0,1);
    lcd.print(setup_screen[currentScreen][1]); lcd.print(" ");
    if (currentScreen == 0) {lcd.print("U"); lcd.print(unit_num);}
  }
  else if (print_mode == 2) {                                         // set parameter screen
    lcd.print(set_parameter_screen[currentScreen][0]); 
    lcd.setCursor(0,1);
    lcd.print(set_parameter_screen[currentScreen][1]);  lcd.print(unit_num); lcd.print(": ");
    lcd.print(praneters[unit_num-1][currentScreen]);
    lcd.print(set_parameter_screen[currentScreen][2]);
  }
  else if (print_mode == 3) {                                         // set num of unit screen
    lcd.print("Set Num Of Unit"); 
    lcd.setCursor(0,1);
    lcd.print(num_of_unit);  lcd.print(" Units");
  }
  else if (print_mode == 4) {                                         // set time screen
    lcd.print(set_time_screen[currentScreen][0]); lcd.print(unit_num);
    lcd.setCursor(0,1);
    if (currentScreen == 3) {
      String day_name;
      switch(set_time_screen_parameters[currentScreen]) {
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
      if (set_time_screen_parameters[currentScreen] < 10) lcd.print("0");
      lcd.print(set_time_screen_parameters[currentScreen]);
    }
    lcd.print(set_time_screen[currentScreen][1]);
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
  *correct_date += "/"; *correct_date += String(month, DEC);
  *correct_date += "/"; *correct_date += String(year, DEC);
  
  if (hour<10) {*correct_time += "0";} *correct_time += String(hour, DEC); *correct_time += ":";
  if (minute<10) {*correct_time += "0";} *correct_time += String(minute, DEC); *correct_time += ":";
  if (second<10) {*correct_time += "0";} *correct_time += String(second, DEC);
}

String lastWaterAsString() {
  String lastWater = "";
  lastWater += String(water[unit_num-1][0]); lastWater += "/";
  lastWater += String(water[unit_num-1][1]); lastWater += "/";
  lastWater += String(water[unit_num-1][2]); lastWater += ",";
  lastWater += String(water[unit_num-1][3]); lastWater += ":";
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
  water[unit][5] = second;
}

