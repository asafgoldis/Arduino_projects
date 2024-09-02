/* connaction:
 *  A0 - Soil Moisture Unit 1
 *  A1 - Soil Moisture Unit 2
 *  A2 - Soil Moisture Unit 3
 *  A3 - Soil Moisture Unit 4
 *  A4 - Lcd - SDA
 *  A5 - Lcd - SCL
 *  A6 - Soil Moisture Unit 5
 *  A7 - Light Sensor
 *  D2 - Up Butten
 *  D3 - Next Butten
 *  D4 - Set Butten
 *  D5 - CLK - RST
 *  D6 - CLK - DAT
 *  D7 - CLK - CLK
 *  D8 - Air Temperature & Humidity
 *  D9 - Relay Unit 1
 *  D10 - Relay Unit 2
 *  D11 - Relay Unit 3
 *  D12 - Relay Unit 4
 *  D13 - Relay Unit 5
*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include <dht.h>
#include <stdio.h>
#include <DS1302.h>

#define DHT11_PIN 8
#define LIGHT_PIN A7
#define SET_PIN 4
#define NEXT_PIN 3
#define UP_PIN 2

LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7);  // Set the LCD I2C address
dht DHT;

int num_of_unit=1;
const int max_num_of_unit=5;

int next = 0;     // for control
int Display=1;
int cuonter=1;
int delta=5;

const int SOIL_PIN[max_num_of_unit]={A0, A1, A2, A3, A6};
const int RELAY_PIN[max_num_of_unit]={9, 10, 11, 12, 13};

double light_meas;
double soil_meas[max_num_of_unit];

//            D_name date/month/year  HHH:MMM:SSS   
int DT[7]= {  1,    1,   1,   2017,  12, 0, 30,};
String day_name[7]={"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

//       soil, air, light, min time between water, max water time
//unit1 |     |    |     |                       |
//unit2 |     |    |     |                       |
//unit3 |     |    |     |                       |
//unit4 |     |    |     |                       |
//unit5 |     |    |     |                       |
unsigned int praneter[max_num_of_unit][5] = {25, 75, 75, 5, 1,
                                             25, 75, 75, 5, 1,
                                             25, 75, 75, 5, 1,
                                             25, 75, 75, 5, 1,
                                             25, 75, 75, 5, 1,};

//       day, month, year, HH,   MM,  control
//unit1 |    |      |     |     |    |
//unit2 |    |      |     |     |    |
//unit3 |    |      |     |     |    |
//unit4 |    |      |     |     |    |
//unit5 |    |      |     |     |    |
unsigned int water[max_num_of_unit][6] = {31, 12, 2016, 12, 30, LOW,
                                          31, 12, 2016, 12, 30, LOW,
                                          31, 12, 2016, 12, 30, LOW,
                                          31, 12, 2016, 12, 30, LOW,
                                          31, 12, 2016, 12, 30, LOW,};

//       water start, water stop
//unit1 |           |
//unit2 |           |
//unit3 |           |
//unit4 |           |
//unit5 |           |
unsigned long water_SS[max_num_of_unit][2];

byte Celsius[8] = {
  0b11000,
  0b11000,
  0b00000,
  0b01110,
  0b10000,
  0b10000,
  0b10000,
  0b01110
};

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

namespace {
// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet: http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 5;  // Chip Enable - RST
const int kIoPin   = 6;  // Input/Output - DAT
const int kSclkPin = 7;  // Serial Clock - CLK

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}

void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %02d-%02d-%04d %02d:%02d:%02d",
           day.c_str(),
           t.date, t.mon, t.yr,
           t.hr, t.min, t.sec);
/*
  // Print the formatted string to serial so we can see the time.
  Serial.print(buf);
  Serial.println(",   ");
*/
  DT[0] = t.day;
  DT[1] = t.date;
  DT[2] = t.mon;
  DT[3] = t.yr;
  DT[4] = t.hr;
  DT[5] = t.min;
  DT[6] = t.sec;
}
}  // namespace

//*********************************************************************************************//

void setTime() {
  rtc.writeProtect(false);
  rtc.halt(false);
  int out = 0;
  do {
  while (digitalRead(UP_PIN)==HIGH && digitalRead(NEXT_PIN)==HIGH);

  if (digitalRead(UP_PIN)==LOW && next==0){DT[0]++;           // D_name
    if (DT[0]>6) DT[0]=0;
  }
  else if (digitalRead(UP_PIN)==LOW && next==1){DT[1]++;       //date
    if (DT[1]>31) DT[1]=1;
  }
  else if (digitalRead(UP_PIN)==LOW && next==2){DT[2]++;       //month
    if (DT[2]>12) DT[2]=1;
  }
  else if (digitalRead(UP_PIN)==LOW && next==3){DT[3]++;       //year
    if (DT[3]>2040) DT[3]=2015;
  }  
  else if (digitalRead(UP_PIN)==LOW && next==4){DT[4]++;       //HHH
    if (DT[4]>23) DT[4]=0;
  }
  else if (digitalRead(UP_PIN)==LOW && next==5){DT[5]++;       //MMM
    if (DT[5]>59) DT[5]=0;
  }
  else if (digitalRead(UP_PIN)==LOW && next==6){DT[6]++;       //SSS
    if (DT[6]>59) DT[6]=0;
  }
  else if (digitalRead(NEXT_PIN)==LOW){next++;
    if (next>6){
      next=0;
      out=1;
    }
  }
/*
  if (next==0)Serial.print("day name: ");
  else if (next==1)Serial.print("day:      ");
  else if (next==2) Serial.print("month:    ");
  else if (next==3) Serial.print("year:     ");
  else if (next==4) Serial.print("hour:      ");
  else if (next==5) Serial.print("min:      ");
  else Serial.print("sec:     ");
    Serial.print("    "); Serial.print(day_name[DT[0]]);
    Serial.print(",         "); Serial.print(DT[1]);
    Serial.print("/"); Serial.print(DT[2]);
    Serial.print("/"); Serial.print(DT[3]);
    Serial.print(",         "); Serial.print(DT[4]);
    Serial.print(":"); Serial.print(DT[5]);
    Serial.print(":"); Serial.println(DT[6]);
*/
  lcd.clear();
  lcd.setCursor(0,0);
  if (next==0){
    lcd.print("Set Day Name:");
    lcd.setCursor(0,1);
    lcd.print(day_name[DT[0]]);
  }
  else if (next==1 || next==2 || next==3){
    if (next==1) lcd.print("Set The Day:");
    if (next==2) lcd.print("Set The Month:");
    if (next==3) lcd.print("Set The Year:");
    lcd.setCursor(0,1);
    lcd.print("  "); lcd.print(DT[1]); lcd.print("/"); lcd.print(DT[2]); lcd.print("/"); lcd.print(DT[3]);
  }
  else if (next==4 || next==5 || next==6){
    if (next==4) lcd.print("Set The Hour:");
    if (next==5) lcd.print("Set The Min:");
    if (next==6) lcd.print("Set The Sec:");
    lcd.setCursor(0,1);
    lcd.print("  "); lcd.print(DT[4]); lcd.print(":"); lcd.print(DT[5]); lcd.print(":"); lcd.print(DT[6]);
  }

  while (digitalRead(UP_PIN)==LOW || digitalRead(NEXT_PIN)==LOW);
  } while (out!=1); 

  if (DT[0]==0) {
    Time t(DT[3],DT[2], DT[1], DT[4], DT[5], DT[6], Time::kSunday);
    // Set the time and date on the chip.
    rtc.time(t);
  }
  else if (DT[0]==1) {
    Time t(DT[3],DT[2], DT[1], DT[4], DT[5], DT[6], Time::kMonday);
    rtc.time(t);
  }
  else if (DT[0]==2) {
    Time t(DT[3],DT[2], DT[1], DT[4], DT[5], DT[6], Time::kTuesday);
    rtc.time(t);
  }
  else if (DT[0]==3) {
    Time t(DT[3],DT[2], DT[1], DT[4], DT[5], DT[6], Time::kWednesday);
    rtc.time(t);
  }
  else if (DT[0]==4){
    Time t(DT[3],DT[2], DT[1], DT[4], DT[5], DT[6], Time::kThursday);
    // Set the time and date on the chip.
    rtc.time(t);
  }
  else if (DT[0]==5){
    Time t(DT[3],DT[2], DT[1], DT[4], DT[5], DT[6], Time::kFriday);
    rtc.time(t);
  }
  else if (DT[0]==6) {
    Time t(DT[3],DT[2], DT[1], DT[4], DT[5], DT[6], Time::kSaturday);
    rtc.time(t);
  }
} //setTime

//*********************************************************************************************//

void setValou(int unit_num) {
  int out = 0;
  do {
  while (digitalRead(UP_PIN)==HIGH && digitalRead(NEXT_PIN)==HIGH);
  if (digitalRead(UP_PIN)==LOW){
    if (next==3) {                                                     // min between water time
      praneter[unit_num][next] = praneter[unit_num][next]+5;
      if (praneter[unit_num][next]>120) {
        praneter[unit_num][next]=0;
        lcd.clear();
      }
    }
    else if (next==4) {                                                // max water time
      praneter[unit_num][next] = praneter[unit_num][next]+1;
      if (praneter[unit_num][next]>30) {
        praneter[unit_num][next]=1;
        lcd.clear();
      }
    }
    else{                                                              // soil, air, light
      praneter[unit_num][next] = praneter[unit_num][next]+5;
      if (praneter[unit_num][next]>100) {
        praneter[unit_num][next]=0;
        lcd.clear();
      }
    }
  }
  else if (digitalRead(NEXT_PIN)==LOW){
    next++;
    if (next>4){
      next=0;
      out=1;
    }
  }
  lcd.clear();
  lcd.setCursor(0,0);
  if (next==0) {
//    Serial.print("soil moisture:        ");
    lcd.print("Soil Moisture U"); lcd.print(unit_num+1);lcd.print(":");
    lcd.setCursor(0,1);
    lcd.print(praneter[unit_num][next]);
    lcd.print("%");
  }  
  else if (next==1) {
 //   Serial.print("air humidity:        ");
    lcd.print("Air Humidity U"); lcd.print(unit_num+1);lcd.print(":");
    lcd.setCursor(0,1);
    lcd.print(praneter[unit_num][next]);
    lcd.print("%");
  }
  else if (next==2) {
//    Serial.print("light:        ");
    lcd.print("Light U"); lcd.print(unit_num+1); lcd.print(":");
    lcd.setCursor(0,1);
    lcd.print(praneter[unit_num][next]);
    lcd.print("%");
  }
  else if (next==3) {
  //  Serial.print("min time:        ");
    lcd.print("Min Time Between");
    lcd.setCursor(0,1);
    lcd.print("Watering U"); lcd.print(unit_num+1); lcd.print(": "); lcd.print(praneter[unit_num][next]);
  }
  else if (next==4) {
  //  Serial.print("max water time:        ");
    lcd.print("Max Watering");
    lcd.setCursor(0,1);
    lcd.print("Time U"); lcd.print(unit_num+1); lcd.print(": "); lcd.print(praneter[unit_num][next]);
  }
  
//  Serial.print(praneter[unit_num][1]); Serial.print("%,           "); Serial.println(praneter[unit_num][0]);
  
  while (digitalRead(UP_PIN)==LOW || digitalRead(NEXT_PIN)==LOW);
  } while (out!=1);
}    //setValou

//*********************************************************************************************//

int unitSelect() {
  int num = 1;
  int out = 0;
  do {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Select Unit: "); lcd.print(num); 
    lcd.setCursor(0,1);
    lcd.print("NEXT=OK,     UP");
    
    while (digitalRead(UP_PIN)==HIGH && digitalRead(NEXT_PIN)==HIGH);
    if (digitalRead(UP_PIN)==LOW){
      num++;
      if (num>num_of_unit) {
        num=1;
      }
      lcd.setCursor(0,0);
      lcd.print("Select Unit: "); lcd.print(num);
    }
    else if (digitalRead(NEXT_PIN)==LOW) {
      out=1;
    }
    while (digitalRead(UP_PIN)==LOW || digitalRead(NEXT_PIN)==LOW); 
  } while (out!=1);
  return num;
}    //unit select

//*********************************************************************************************//

int Select_num_of_unit() {
  int out = 0;
  num_of_unit=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Num of Unit? "); lcd.print(num_of_unit); 
  lcd.setCursor(0,1);
  lcd.print("NEXT=OK,     UP");
  do {
    while (digitalRead(UP_PIN)==HIGH && digitalRead(NEXT_PIN)==HIGH);
    if (digitalRead(UP_PIN)==LOW){
      num_of_unit++;
      if (num_of_unit>5) {
        num_of_unit=0;
      }
      lcd.setCursor(0,0);
      lcd.print("Num of Unit? "); lcd.print(num_of_unit);
    }
    else if (digitalRead(NEXT_PIN)==LOW) {
      out=1;
    }
    while (digitalRead(UP_PIN)==LOW || digitalRead(NEXT_PIN)==LOW); 
  } while (out!=1);
  lcd.clear();
}    // select num of unit

//*********************************************************************************************//

void SetupManu(){
  int num=0;
  int out=0;
/*
    Serial.println(""); Serial.println("");
    Serial.println("for valou setup peress up key");
    Serial.println("for time&date setup peress next key");
    Serial.println("to cancel press set key");
    Serial.println("");
*/
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("Setup Menu:");
    delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set Parameter?");
    lcd.setCursor(0,1);
    lcd.print("CANCEL, OK, UP");

    while (digitalRead(SET_PIN)==LOW);
    int temp=1;
    lcd.setCursor(0,0);
    lcd.print("Set Parameter?");
    do {
      while (digitalRead(UP_PIN)==HIGH && digitalRead(NEXT_PIN)==HIGH && digitalRead(SET_PIN)==HIGH);
      lcd.setCursor(0,1);
      lcd.print("CANCEL, OK, UP");
      if (digitalRead(UP_PIN)==LOW){
        while (digitalRead(UP_PIN)==LOW);
        temp++;
        if (temp>3) temp=1;
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("CANCEL, OK, UP");
        lcd.setCursor(0,0);
        if (temp==1) lcd.print("Set Parameter?");
        if (temp==2) lcd.print("Set Time?");
        if (temp==3) lcd.print("Set Num of Unit?");
      }
      else if (digitalRead(NEXT_PIN)==LOW){
        while (digitalRead(NEXT_PIN)==LOW);
        out=1;
      }
      else if (digitalRead(SET_PIN)==LOW) {
        while (digitalRead(SET_PIN)==LOW);
        out=1;
        temp=0;
      }
    } while (out!=1);
    
    lcd.clear();
    if (temp==2){         //setTime
/*
      Serial.println("setTime on");
      Serial.println("set       dey name,   dey/month/year,   hour:min:sec");

      Serial.print("day name:     ");
      Serial.print(day_name[DT[0]]); Serial.print(",         ");
      Serial.print(DT[1]); Serial.print("/");
      Serial.print(DT[2]); Serial.print("/");
      Serial.print(DT[3]); Serial.print(",         ");
      Serial.print(DT[4]); Serial.print(":");
      Serial.print(DT[5]); Serial.print(":");
      Serial.println(DT[6]);
*/
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Time Setup Menu:");
      delay (1000);
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Dey name:");
      lcd.setCursor(0,1);
      lcd.print(day_name[DT[0]]);
      setTime();
    }
    else if (temp==1 && num_of_unit!=0){               //setValou
      num = unitSelect();
      lcd.clear();
/*
      Serial.println("setValou on");
      Serial.println("set            air humidity,   soil moisture");
      Serial.print("air humidity:        "); Serial.print(praneter[num][1]);
      Serial.print("%,           "); Serial.println(praneter[num][0]);
*/
      if (num>0) {
        lcd.setCursor(0,0);
        lcd.print("Parameter Setup");
        lcd.setCursor(2,1);
        lcd.print("Menu: Unit "); lcd.print(num);
        delay(2000);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Soil Moisture U"); lcd.print(num); lcd.print(":");
        lcd.setCursor(0,1);
        lcd.print(praneter[num-1][0]);lcd.print("%"); 
        setValou(num-1);
      }
    }
    else if (temp==3){               //set num of unit
      Select_num_of_unit();
    }
    lcd.clear();
    cuonter=1;
    num=1;
}

//*********************************************************************************************//

void setup(){
  Serial.begin(9600);
  pinMode (RELAY_PIN[0],OUTPUT);
  pinMode (RELAY_PIN[1],OUTPUT);
  pinMode (RELAY_PIN[2],OUTPUT);
  pinMode (RELAY_PIN[3],OUTPUT);
  pinMode (RELAY_PIN[4],OUTPUT);
  
  pinMode (SET_PIN, INPUT_PULLUP);
  pinMode (UP_PIN, INPUT_PULLUP);
  pinMode (NEXT_PIN, INPUT_PULLUP);
  

  // set up the LCD's number of columns and rows:
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.createChar(0, Celsius);
  lcd.createChar(1, Tipa);

  // set the cursor to (0,0):
  lcd.setCursor(0, 0);
  //lcd.print("Water System");
  lcd.write(1); lcd.print(" Water System "); lcd.write(1);
  lcd.setCursor(1, 1);
  lcd.print("By Asaf Goldis");
  delay(2000);
  lcd.clear();
// DEL
  Serial.println("");
  Serial.println("Water System by Asaf Goldis");
  Serial.println("");
//
  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  //rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  //Time t(2013, 9, 22, 1, 38, 50, Time::kSunday);

  // Set the time and date on the chip.
  //rtc.time(t);
}

//*********************************************************************************************//

void loop() {
  unsigned long currentMillis = millis(); 
  unsigned long currentMinutes = currentMillis/60000;

  if (num_of_unit>=1) {soil_meas[0]=analogRead(SOIL_PIN[0]); soil_meas[0]=soil_meas[0]*100/1023;}
  if (num_of_unit>=2) {soil_meas[1]=analogRead(SOIL_PIN[1]); soil_meas[1]=soil_meas[1]*100/1023;}
  if (num_of_unit>=3) {soil_meas[2]=analogRead(SOIL_PIN[2]); soil_meas[2]=soil_meas[2]*100/1023;}
  if (num_of_unit>=4) {soil_meas[3]=analogRead(SOIL_PIN[3]); soil_meas[3]=soil_meas[3]*100/1023;}
  if (num_of_unit>=5) {soil_meas[4]=analogRead(SOIL_PIN[4]); soil_meas[4]=soil_meas[4]*100/1023;}
  light_meas=analogRead(LIGHT_PIN); light_meas=light_meas*100/1023;

  if (digitalRead(SET_PIN)==LOW){                      //setup menu
    while (digitalRead(SET_PIN)==LOW);
    SetupManu();
  }
  if (digitalRead(UP_PIN)==LOW) {                      //torn display on
    while (digitalRead(UP_PIN)==LOW);
    //lcd.display();
    lcd.setBacklight(HIGH);
  }
  if (digitalRead(NEXT_PIN)==LOW) {                    //torn display off
    while (digitalRead(NEXT_PIN)==LOW);
    //lcd.noDisplay();
    lcd.setBacklight(LOW);
  }

  if (Display>=1 && Display<=num_of_unit){ //               soil moisture unit 1+2+3+4+5
    lcd.setCursor(2,0);
    lcd.print("Soil Moisture");
    lcd.setCursor(1,1);
    lcd.print("Unit "); lcd.print(Display); lcd.print(": "); lcd.print(soil_meas[Display-1]); lcd.print("%");
    cuonter++;
    if (cuonter>13) {
      cuonter=1;
      lcd.clear();
      Display++;
    }
  }
  else if (Display>=num_of_unit+1 && Display<=num_of_unit*2){ // last waterd 1+2+3+4+5
    lcd.setCursor(0,0);
    lcd.print("Last Watering U"); lcd.print(Display-num_of_unit); lcd.print(":");
    lcd.setCursor(0,1);
    lcd.print(water[Display-num_of_unit-1][0]); lcd.print("/"); lcd.print(water[Display-num_of_unit-1][1]); lcd.print("/"); lcd.print(water[Display-num_of_unit-1][2]); 
    lcd.print (","); 
    if (water[Display-num_of_unit-1][3]>9) lcd.print(water[Display-num_of_unit-1][3]);
    else {
      lcd.print(water[Display-num_of_unit-1][3]-water[Display-num_of_unit-1][3]%10); lcd.print(water[Display-num_of_unit-1][3]%10);
    }
    lcd.print(":"); 
    if (water[Display-num_of_unit-1][4]>9) lcd.print(water[Display-num_of_unit-1][4]);
    else {
      lcd.print(water[Display-num_of_unit-1][4]-water[Display-num_of_unit-1][4]%10); lcd.print(water[Display-num_of_unit-1][4]%10);
    }
    cuonter++;
    if (cuonter>13) {
      cuonter=1;
      lcd.clear();
      Display++;
    }
  }
  else if (Display==num_of_unit*2+1){                      // air temperature and humidity
    lcd.setCursor(2,0);
    lcd.print("Air Temperature & Humidity");
    lcd.scrollDisplayLeft();
    lcd.setCursor(cuonter,1);
    lcd.print(DHT.temperature); lcd.write(byte(0)); lcd.print(",  "); lcd.print(DHT.humidity); lcd.print("%");
    cuonter++;
    if (cuonter>24) {
      cuonter=1;
      lcd.clear();
      Display++;
    }
  }
  else if (Display==num_of_unit*2+2){            // light sensor
    lcd.setCursor(5,0);
    lcd.print("Light:");
    lcd.setCursor(5,1);
    lcd.print(light_meas); lcd.print("%");
    cuonter++;
    if (cuonter>13) {
      cuonter=1;
      lcd.clear();
      Display++;
    }
  }
  else {                            // Date & Time
    lcd.setCursor(2,0);
    lcd.print("Date & Time");
    lcd.setCursor(0,1);
    lcd.print(DT[1]); lcd.print("/"); lcd.print(DT[2]); lcd.print("/"); lcd.print(DT[3]);
    lcd.print(", "); 
    if (DT[4]>9) lcd.print(DT[4]);
    else {
      lcd.print(DT[4]-DT[4]%10); lcd.print(DT[4]%10);
    }
    lcd.print(":"); 
    if (DT[5]>9) lcd.print(DT[5]);
    else {
      lcd.print(DT[5]-DT[5]%10); lcd.print(DT[5]%10);
    }
    cuonter++;
    if (cuonter>10) {
      cuonter=1;
      Display=1;
      lcd.clear();
    }
  }
  printTime();

  int chk = DHT.read11(DHT11_PIN);
  
/*
  Serial.print("Temperature = "); Serial.print(DHT.temperature);
  Serial.print(",  Humidity = "); Serial.print(DHT.humidity);
  Serial.print(",  soil moisture = ");  Serial.print(soil_meas[0]);
  Serial.print(",  relay = ");

  if (digitalRead(RELAY_PIN[0])==HIGH)Serial.println("on ");
  else Serial.println("off"); 
  Serial.print(", ***RELAY is on if Air Humidity < "); Serial.print(praneter[0][1]); Serial.print(", and Soil moisture < "); Serial.print(praneter[0][0]); Serial.print(", ");

Serial.print(", currentMinutes = "); Serial.print(currentMinutes);
Serial.print(", water_stop = "); Serial.print(water_SS[0][1]);
Serial.print(", praneter = "); Serial.print(praneter[0][3]);
Serial.print(", water_start = "); Serial.print(water_SS[0][0]);
Serial.print(", RELAY_PIN = "); Serial.print(digitalRead(RELAY_PIN[0]));
Serial.print(", max watre time= "); Serial.println(praneter[0][4]);
*/
  for (int j=0; j<num_of_unit; j++){
    if (water[j][5]==LOW) delta=-5;                            //histerzis
    else delta=5;
    
    if (soil_meas[j]<praneter[j][0]+delta &&
      DHT.humidity<praneter[j][1]+delta &&
      light_meas<praneter[j][2]+delta &&
      currentMinutes-water_SS[j][1]>=praneter[j][3]) {  //unit 1+2+3+4+5
      
      if (water[j][5]==LOW) water_SS[j][0]=currentMinutes;
      if (currentMinutes-water_SS[j][0]<=praneter[j][4]){
        digitalWrite(RELAY_PIN[j],HIGH);
        water[j][5]=HIGH; 
      }
      else {
        water_SS[j][1]=currentMinutes;
        for (int i=0; i<5;) {
          water[j][i]=DT[i+1];
          i++;
        }
        digitalWrite (RELAY_PIN[j],LOW);
        water[j][5]=LOW;
      }
    }
    else {
      if (water[j][5]==HIGH) {
        water_SS[j][1]=currentMinutes;
        for (int i=0; i<5;) {
          water[j][i]=DT[i+1];
          i++;
        }
      }
      digitalWrite (RELAY_PIN[j],LOW);
      water[j][5]=LOW;
    }
  }
  delay(500);
}

