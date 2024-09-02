#include <Servo.h>
Servo myServoX;
Servo myServoY;

const int ServoXPin = 9;
const int ServoYPin = 10;
//const int ledPin = 13;
const int delayTime = 4;

int centroidX;
int centroidY;
//int angleX;
//int angleY;
  
 void setup(){
  myServoX.attach(ServoXPin);
  myServoY.attach(ServoYPin);
  //pinMode(ledPin,OUTPUT);
  Serial.begin(115200);
  Serial.println("Ready");
}

 void loop() {
  //digitalWrite(ledPin,LOW); //turn off LED
if (Serial.available() > 0) {
  //digitalWrite(ledPin,HIGH); //flash LED everytime data is available
  
  centroidX = Serial.read();
  delay(delayTime);
  centroidY = Serial.read();
  //delay(delayTime);
  
  myServoX.write(centroidX);
  myServoY.write(centroidY);
  //delay(delayTime);
  
  //angleX = myServoX.read();
  //angleY = myServoY.read();
  
  //Serial.println(angleX); //print data
  //delay(delayTime);
  //Serial.println(angleY); //print data
  //delay(delayTime);
}
//delay(10);
}
