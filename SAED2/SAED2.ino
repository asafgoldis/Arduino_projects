//***************************************************************************************************************************//
//                                            SAED V7
//***************************************************************************************************************************//
#include <LiquidCrystal_I2C.h> // Driver Library for the LCD Module
#include<Servo.h>
#include <dht11.h>

//***************************************************************************************************************************//
// Define IO Pin
#define DHT_PIN 2
#define SERVO_PIN 3
#define LED_PIN 7
#define GAS_PIN 8
#define FAN_PIN 9
#define FLAME_PIN 10
#define BUZZ_PIN 11
#define LDR_PIN A0

// Define constant
#define LiquidCrystal_I2C_ADDRESS 0x27

#define LED_ON LOW
#define LED_OFF HIGH

#define FAN_ON HIGH
#define FAN_OFF LOW

#define BUZZ_ON HIGH
#define BUZZ_OFF LOW

#define DOOR_OPEN 90
#define DOOR_CLOSE 0

bool FlameBuzz = false;
bool GasBuzz = false;

// initialize
dht11 DhtSensor;
LiquidCrystal_I2C lcd(LiquidCrystal_I2C_ADDRESS,20,4); // Adjust to (0x27,20,4) for 20x4 LCD
Servo DoorServo;

//***************************************************************************************************************************//
void setup() {
  pinMode(GAS_PIN,INPUT);             // mq2
  pinMode(FLAME_PIN,INPUT);           // flame
  pinMode(LED_PIN,OUTPUT);            // led
  pinMode(BUZZ_PIN,OUTPUT);           // zamzam
  pinMode(FAN_PIN,OUTPUT);          // motor
  DoorServo.attach(SERVO_PIN);
  
  digitalWrite(LED_PIN, LED_ON);       // led on
  digitalWrite(BUZZ_PIN, BUZZ_ON);     // zamzam on 
  digitalWrite(FAN_PIN, FAN_ON);   // motor on
  DoorServo.write(DOOR_OPEN);

  lcd.init();                         // initialize the lcd 
  lcd.backlight();                    // Turn on the backlight 
  lcd.home();
  lcd.setCursor(5, 0);                // Set the cursor on the first column and first row.
  lcd.print("bait haham");            // Print the string "Hello World!"
  lcd.setCursor(0, 1);                // Set cursor to 2nd column and 2nd row (counting starts at 0)
  lcd.print("01234567890123456789");
  lcd.setCursor(5,2);
  lcd.print("SAED & BAR");
  lcd.setCursor(7,3);
  lcd.print("test 7");
  delay(1500);

  digitalWrite(LED_PIN, LED_OFF);      // led off
  digitalWrite(BUZZ_PIN, BUZZ_OFF);    // zamzam off
  digitalWrite(FAN_PIN, FAN_OFF);  // motor off
  DoorServo.write(DOOR_CLOSE);
  delay(500);
}
//***************************************************************************************************************************//
void loop() {
  lcd.clear();

  GasCheck();
  FlameCheck();
  Buzz_Door_Activate();
  LdrCheck();
  DhtCheck();
  
  delay(500);
}
//***************************************************************************************************************************//
void GasCheck() {
  lcd.setCursor(0, 1);
  if(digitalRead(GAS_PIN)) {
    lcd.print("NO GAS, ");
    GasBuzz = false;
  }
  else {
   lcd.print("GAS,    ");
   GasBuzz = true;
  }
}
//***************************************************************************************************************************//
void FlameCheck() {
  lcd.setCursor(9, 1);
   if(digitalRead(FLAME_PIN)) {
    lcd.print("NO flame   ");
    FlameBuzz = false;
  }
  else {
     lcd.print("DANGER!!!  ");
     FlameBuzz = true;
  }
}
//***************************************************************************************************************************//
void Buzz_Door_Activate() {
  if (FlameBuzz||GasBuzz) {
    digitalWrite(BUZZ_PIN, BUZZ_ON);             // zamzam on
    DoorServo.write(DOOR_OPEN);                 // door open
  }
  else {
    digitalWrite(BUZZ_PIN, BUZZ_OFF);            // zamzam off
    DoorServo.write(DOOR_CLOSE);                // door close
  }
}
//***************************************************************************************************************************//
void LdrCheck() {
  if(analogRead(LDR_PIN) < 400)
    digitalWrite(LED_PIN, LED_OFF);              // led off
  else
    digitalWrite(LED_PIN, LED_ON);               // led on
}
//***************************************************************************************************************************//
void DhtCheck() {
  int chk = DhtSensor.read(DHT_PIN);
  lcd.setCursor(0,2);
  lcd.print("Humidity (%): ");
  lcd.print((float)DhtSensor.humidity, 2);
  lcd.setCursor(0,3);
  lcd.print("Temp(C): ");
  lcd.print((float)DhtSensor.temperature, 2);
  
  if(DhtSensor.temperature > 23)
    digitalWrite(FAN_PIN, FAN_ON);         // motor on
  else
    digitalWrite(FAN_PIN, FAN_OFF);        // motor off
  
}
//***************************************************************************************************************************//
