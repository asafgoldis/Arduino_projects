#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(3,2); // RX, TX

#define PHONE_NUM +972505577708
#define RELAY_PIN_1 8
#define RELAY_PIN_2 7
#define RELAY_PIN_3 6
#define RELAY_PIN_4 5
#define LED_PIN 13
#define BUTTON_PIN 9


String Sms;
String Phone;
String Date;
String Time;
int buttonState = 0;
int Cmd_num = 99;
String Commands[10] = {"relay1on", "relay2on", "relay3on", "relay4on",
                       "relay1off", "relay2off", "relay3off", "relay4off", 
                       "ledOn",
                       "ledOff"};

void setup() {
    gprsSerial.begin(9600); // GPRS shield baud rate
    Serial.begin(9600);

    //gprsSerial.println("AT"); //Once the handshake test is successful, it will back to OK
    //gprsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
    //gprsSerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    
    pinMode(RELAY_PIN_1, OUTPUT); digitalWrite(RELAY_PIN_1, HIGH);
    pinMode(RELAY_PIN_2, OUTPUT); digitalWrite(RELAY_PIN_2, HIGH);
    pinMode(RELAY_PIN_3, OUTPUT); digitalWrite(RELAY_PIN_3, HIGH);
    pinMode(RELAY_PIN_4, OUTPUT); digitalWrite(RELAY_PIN_4, HIGH);
    pinMode(LED_PIN, OUTPUT); digitalWrite(LED_PIN, LOW);
    
    pinMode(BUTTON_PIN, INPUT);
    Serial.println("Start");
    delay(500);
}

void loop() {
    if (gprsSerial.available()) { // if the shield has something to say
      GetMsg();
      Cmd_num = Compare(Sms);
      Action(Cmd_num);
    }
    
    //led();
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

void Action(int cmd_num) {
  if      (cmd_num == 0) digitalWrite(RELAY_PIN_1, LOW);
  else if (cmd_num == 1) digitalWrite(RELAY_PIN_2, LOW);
  else if (cmd_num == 2) digitalWrite(RELAY_PIN_3, LOW);
  else if (cmd_num == 3) digitalWrite(RELAY_PIN_4, LOW);
  
  else if (cmd_num == 4) digitalWrite(RELAY_PIN_1, HIGH);
  else if (cmd_num == 5) digitalWrite(RELAY_PIN_2, HIGH);
  else if (cmd_num == 6) digitalWrite(RELAY_PIN_3, HIGH);
  else if (cmd_num == 7) digitalWrite(RELAY_PIN_4, HIGH);
  
  else if (cmd_num == 8) digitalWrite(LED_PIN, HIGH);
  else if (cmd_num == 9) digitalWrite(LED_PIN, LOW);
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
/*
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
*/
int Compare(String String1) {
  String String2 = "";
  int str1Length = String1.length()-1;
  int str2Length = 0;
  char char1, char2;
  int cmd_num = 99;
  
  for (int cmd=0; cmd<sizeof(Commands); cmd++) {
    String2 = Commands[cmd];
    str2Length = String2.length();
    
    if (str1Length == str2Length) {
      for (int i=0; i<str1Length; i++) {
        char1 = String1.charAt(i);
        char2 = String2.charAt(i);
        if (char1 == char2)
          cmd_num = cmd;
        else {
          cmd_num = 99;
          break;
        }
      }
      if (cmd_num != 99)
        return cmd_num;
    }

      /*
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
      */
      //return is_equals;
    }
  return 99;
}
/*
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
*/
