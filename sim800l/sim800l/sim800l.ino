
#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial sim800l(3, 2); // RX, TX
float sensorValue;
const int buttonPin = 9;
int buttonState = 0;
unsigned char buffer[64]; // buffer array for data recieve over serial port
int buffercunt;
int count=0;     // counter for buffer array

void setup() {
  pinMode(buttonPin, INPUT); 
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  sim800l.begin(9600);
  Serial.begin(9600);   
  delay(500);
  digitalWrite(6, LOW);
  sim800l.println("AT"); //Once the handshake test is successful, it will back to OK
  //updateSerial();
  
  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  //updateSerial();
  sim800l.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  //updateSerial();
  
}
 
void loop() {
  updateSerial();
  buttonState = digitalRead(buttonPin);
 
  if (buttonState == 0) {
    sensorValue = analogRead(A0);
    delay(300);
    Serial.println(sensorValue);
    SendTextMessage(); 
 }

   // if (Serial.available())            // if data is available on hardwareserial port ==> data is comming from PC or notebook
    //  sim800l.write(Serial.read());       // write it to the GPRS shield
/*
if (Serial.available()) // if there is incoming serial data
    Serial.println("123");
     switch(Serial.read()) { // read the character
       case 'On': // if the character is 't'
         digitalWrite(6, HIGH);
         break;
       case 'Off': // if the character is 'd'
         digitalWrite(6, LOW);
         break;
     }

  */
}
 
void SendTextMessage() {
  Serial.println("Sending Text...");
  sim800l.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
 
  sim800l.print("AT+CMGS=\"972505577708\"\r");  
  delay(200);

  sim800l.print("The temperature is: ");
  sim800l.print(sensorValue);
  sim800l.print(" degrees C");
  sim800l.print("\r"); //the content of the message
  delay(500);
  sim800l.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);
}

void updateSerial() {
  delay(500);
  //while (Serial.available()) {
  //  sim800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  //}
  while(sim800l.available()) {
    Serial.write(sim800l.read());//Forward what Software Serial received to Serial Port
   
  }
}
