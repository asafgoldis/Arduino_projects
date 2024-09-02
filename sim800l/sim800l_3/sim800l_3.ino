#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(3,2);

#define PHONE_NUM 972505577708

String Sms;
String Phone;
String Date;
String Time;

#define LED_PIN 6
//#define BUTTON_PIN 7
//int buttonState = 0;

void setup() {
    gprsSerial.begin(9600); // GPRS shield baud rate
    Serial.begin(9600);
    
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
//    pinMode(BUTTON_PIN, INPUT);
    delay(500);
}

void loop() {
    if (gprsSerial.available()) { // if the shield has something to say
      GetMsg();
    }
    
    led();
    delay(500);
/*
    buttonState = digitalRead(buttonPin);
    if (buttonState == 0) {
      delay(300);
      SendTextMessage(); 
   }
*/
}

void GetMsg() {
  Serial.println("available...");
  unsigned char inData[64];
  int cunt = 0;
  Phone = "";
  Date = "";
  Time = "";
  Sms = "";

  while(gprsSerial.available()) {
    inData[cunt] = gprsSerial.read();
    cunt++;
    delay(10);
  }
  
  for (int i = 0; i<cunt; i++){
    if (i>8 && i<=21)
      Phone += char(inData[i]);
    else if (i>27 && i<=35)
      Date += char(inData[i]);
    else if (i>36 && i<=44)
      Time += char(inData[i]);
    else if (i>=52)
      Sms += char(inData[i-1]);
  }
  
  //inData += "\0";
  //Serial.println("Phone = "+ Phone);
  //Serial.println("Date = "+ Date);
  //Serial.println("Time = "+ Time);
  //Serial.println("Sms = "+ Sms);
  Serial.println(Sms);
  //Serial.println(Sms.length());
}

/*
void SendTextMessage() {
  Serial.println("Sending Text...");
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
 
  gprsSerial.print("AT+CMGS=\"972505577708\"\r");  
  delay(200);

  gprsSerial.print("Alart!! \r");
  gprsSerial.print("PIR is work");
  gprsSerial.print("\r"); //the content of the message
  delay(500);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
  delay(500);
}
*/

void led() {
  //
  if (Compare(Sms, "On"))
  //if (Sms == "On")
    digitalWrite(6, HIGH);
  else if (Compare(Sms, "Off"))
  //else if (Sms == "Off")
    digitalWrite(6, LOW);

  Sms = "";
}

bool Compare(String String1, String String2) {
  int str1Length = String1.length()-1;
  int str2Length = String2.length();
  char char1, char2;
  bool is_equals = false;

  Serial.print("str1Length = "); Serial.print(str1Length); Serial.print(", str2Length = "); Serial.println(str2Length);
    if (str1Length != str2Length) {
      Serial.println("length not equals");
      return false;
    }
    else {
      for (int i=0; i<str1Length; i++) {
        char1 = String1.charAt(i);
        char2 = String2.charAt(i);
        if (char1 == char2)
          is_equals = true;
        else {
          is_equals = false;
          break;
        }
      }
      return is_equals;
    }
  return false;
}
