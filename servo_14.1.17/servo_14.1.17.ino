#include <Servo.h>

Servo myServo;

int const potPin = A0; //analog pin A 
int potVal; //stores value of pot 
int angle = 0; //stores position of servo

void setup() { 
  myServo.attach(8); //tells board which pin the servo is on
  Serial.begin(9600); 
} //end of setup()

void loop() { 
  potVal = analogRead(potPin); 
  Serial.print("potVal: "); 
  Serial.print(potVal);

angle = map(potVal, 0, 1023, 0, 179); //angle is the pot value re-scaled to 0-179 
  Serial.print(", angle: "); 
  Serial.println(angle);
  
  myServo.write(angle); 
  delay(10); 
} //end of loop()
