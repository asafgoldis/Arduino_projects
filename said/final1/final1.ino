
#include <LiquidCrystal_I2C.h> // Driver Library for the LCD Module
#include<Servo.h>
#include <dht11.h>

#define DHT11PIN 2
#define SERVO_PIN 3
#define LED_PIN 7
#define GAS_PIN 8
#define MOTOR_PIN 9
#define FLAME_PIN 10
#define BAZZ_PIN 11
#define LDR_PIN A0

#define LED_ON LOW
#define LED_OFF HIGH

#define MOTOR_ON HIGH
#define MOTOR_OFF LOW

#define BAZZ_ON HIGH
#define BAZZ_OFF LOW

#define DOOR_OPEN 90
#define DOOR_CLOSE 0

dht11 DHT11;
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,20,4); // Adjust to (0x27,20,4) for 20x4 LCD
Servo Myservo;

int mq,flame,ldr;
bool FlameBazz = false;
bool GasBazz = false;

////////////////////////////////////////////
void setup() {
  pinMode(GAS_PIN,INPUT);       //mq2
  pinMode(FLAME_PIN,INPUT);       //flame
  pinMode(LED_PIN,OUTPUT);     //led
  pinMode(BAZZ_PIN,OUTPUT);     //zamzam
  pinMode(MOTOR_PIN,OUTPUT);     //motor
  Myservo.attach(SERVO_PIN);

  digitalWrite(LED_PIN,LED_ON);   //led on
  digitalWrite(BAZZ_PIN,BAZZ_ON);   //zamzam on 
  digitalWrite(MOTOR_PIN,MOTOR_ON);   //motor on
  Myservo.write(DOOR_OPEN);
 
  lcd.begin();          // Initiate the LCD module
  lcd.backlight();     // Turn on the backlight 
  lcd.setCursor(5, 0);          // Set the cursor on the first column and first row.
  lcd.print("bait haham");    // Print the string "Hello World!"
  lcd.setCursor(0, 1);          //Set cursor to 2nd column and 2nd row (counting starts at 0)
  lcd.print("01234567890123456789");
  lcd.setCursor(5,2);
  lcd.print("SAED & BAR");
  lcd.setCursor(7,3);
  lcd.print("test 7");
  delay(1500);

  digitalWrite(LED_PIN,LED_OFF);   //led off
  digitalWrite(BAZZ_PIN,BAZZ_OFF);   //zamzam off
  digitalWrite(MOTOR_PIN,MOTOR_OFF);          // motor off
  Myservo.write(DOOR_CLOSE);
  delay(500);
}
///////////////////////////////////////////
void loop() {
  lcd.clear();

  GasCheck();
  FlameCheck();
  Buzz();
  LdrCheck();
  DhtCheck();
  
  delay(500);
}

void GasCheck() {
  mq=digitalRead(GAS_PIN);
  lcd.setCursor(0, 1);
  if(mq==HIGH) {
    lcd.print("NO GAS, ");
    GasBazz = false;
  }
  else {
   lcd.print("GAS,    ");
   GasBazz = true;
  }
}

void FlameCheck(){
  lcd.setCursor(9, 1);
  flame=digitalRead(FLAME_PIN);
   if(flame==HIGH)  {
    lcd.print("NO flame   ");
    FlameBazz = false;
  }
  else {
     lcd.print("DANGER!!!  ");
     FlameBazz = true;
  }
}

void Buzz() {
  if (FlameBazz||GasBazz) {
    digitalWrite(BAZZ_PIN,BAZZ_ON);      //zamzam on
    Myservo.write(DOOR_OPEN);         //door open
  }
  else {
    Myservo.write(DOOR_CLOSE);           // door close
    digitalWrite(BAZZ_PIN,BAZZ_OFF);    //zamzam off
  }
}

void LdrCheck() {
   ldr=analogRead(LDR_PIN);
  if(ldr<400)
    digitalWrite(LED_PIN,LED_OFF);   //led off
  else
    digitalWrite(LED_PIN,LED_ON);   //led on
}

void DhtCheck() {
  int chk = DHT11.read(DHT11PIN);
  lcd.setCursor(0,2);
  lcd.print("Humidity (%): ");
  lcd.print((float)DHT11.humidity, 2);
  lcd.setCursor(0,3);
  lcd.print("Temp(C): ");
  lcd.print((float)DHT11.temperature, 2);
  
  if(DHT11.temperature>23)
    digitalWrite(MOTOR_PIN,MOTOR_ON);         // motor on
  else
    digitalWrite(MOTOR_PIN,MOTOR_OFF);         //motor off
  
}
