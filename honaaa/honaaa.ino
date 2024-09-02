//***************************************************************************************************************************//
//                                            Remote Control System - HONAAA V1.0
//***************************************************************************************************************************//

//    IO Connaction:
// A0 - MICROSWISH_2_PIN  // for motor 1
// A1 - MICROSWISH_3_PIN  // for motor 2
// A2 - MICROSWISH_4_PIN  // for motor 2
// A3 - NOT CONNECTED
// A4 - NOT CONNECTED
// A5 - NOT CONNECTED
// A6 - NOT CONNECTED
// A7 - NOT CONNECTED
// D0 - NOT CONNECTED 
// D1 - NOT CONNECTED
// D2 - TX
// D3 - RX
// D4 - switch_relay_4
// D5 - switch_relay_3
// D6 - switch_relay_2
// D7 - switch_relay_1
// D8 - motor_relay_4
// D9 - motor_relay_3
// D10 - motor_relay_2
// D11 - motor_relay_1
// D12 - MICROSWISH_1_PIN  // for motor 1
// D13 - NOT CONNECTED
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include <String.h>
//***************************************************************************************************************************//
// Define IO
#define NUM_OF_CMD 16

#define PHONE_NUM +972505577708

#define MICROSWISH_4_PIN A2
#define MICROSWISH_3_PIN A1
#define MICROSWISH_2_PIN A0
#define MICROSWISH_1_PIN 12

#define MOTOR_RELAY_1 11
#define MOTOR_RELAY_2 10
#define MOTOR_RELAY_3 9
#define MOTOR_RELAY_4 8

#define SWITCH_RELAY_1 7
#define SWITCH_RELAY_2 6
#define SWITCH_RELAY_3 5
#define SWITCH_RELAY_4 4

//***************************************************************************************************************************//
SoftwareSerial gprsSerial(3,2);       // RX, TX

int CMD[NUM_OF_CMD];
int time_to_move_1 = 15000; // 15 sec
int time_to_move_2 = 15000; // 15 sec

bool microswish_1_state = LOW;
bool microswish_2_state = LOW;
bool microswish_3_state = LOW;
bool microswish_4_state = LOW;
unsigned long loop_timer_1, loop_timer_2;
bool M1on, M1cw, M2on, M2cw = false;

String SmsToSend;
String Commands[NUM_OF_CMD] = {"M1CW", "M1CCW", "M2CW", "M2CCW", 
                               "Switch1on", "Switch2on", "Switch3on", "Switch4on", 
                               "Switch1off", "Switch2off", "Switch3off", "Switch4off", 
                               "AllSwitchOn", "AllSwitchOff", "Status", "Cmd"};
char Received_SMS;       //Here we store the full received SMS (with phone sending number and date/time) as char
//***************************************************************************************************************************//
void setup() {
  gprsSerial.begin(9600); // GPRS shield baud rate
  Serial.begin(9600);
  
  for (int i=0; i<NUM_OF_CMD; i++) {
    CMD[i] = -1;
  }
  pinMode(SWITCH_RELAY_1, OUTPUT); digitalWrite(SWITCH_RELAY_1, HIGH);
  pinMode(SWITCH_RELAY_2, OUTPUT); digitalWrite(SWITCH_RELAY_2, HIGH);
  pinMode(SWITCH_RELAY_3, OUTPUT); digitalWrite(SWITCH_RELAY_3, HIGH);
  pinMode(SWITCH_RELAY_4, OUTPUT); digitalWrite(SWITCH_RELAY_4, HIGH);
  
  pinMode(MOTOR_RELAY_1, OUTPUT); digitalWrite(MOTOR_RELAY_1, HIGH);
  pinMode(MOTOR_RELAY_2, OUTPUT); digitalWrite(MOTOR_RELAY_2, HIGH);
  pinMode(MOTOR_RELAY_3, OUTPUT); digitalWrite(MOTOR_RELAY_3, HIGH);
  pinMode(MOTOR_RELAY_4, OUTPUT); digitalWrite(MOTOR_RELAY_4, HIGH);
  
  pinMode(MICROSWISH_1_PIN , INPUT_PULLUP);
  pinMode(MICROSWISH_2_PIN , INPUT_PULLUP);

  
  ReceiveMode();                  //Calling the function that puts the SIM800L moduleon receiving SMS mode
  //Serial.println("Start");
  loop_timer_1 = millis();
  loop_timer_2 = loop_timer_1;
  delay(500);
}
//***************************************************************************************************************************//
void loop() {
  //cnt++;
  //Serial.println(cnt);
  if (gprsSerial.available()) {     // if the shield has something to say
    GetMsg();
    Action();
    ReceiveMode();
  }
  
  microswish_1_state = digitalRead(MICROSWISH_1_PIN); // for motor 1
  microswish_2_state = digitalRead(MICROSWISH_2_PIN); // for motor 1
  microswish_3_state = digitalRead(MICROSWISH_3_PIN); // for motor 2
  microswish_4_state = digitalRead(MICROSWISH_4_PIN); // for motor 2

  if (M1on) Direction_1();
  if (M2on) Direction_2();

  delay(200);
}
//***************************************************************************************************************************//
void GetMsg() {
  //Serial.println("available...");
  String RSMS;                              //We add this new variable String type, and we put it in loop so everytime gets initialized
                                            //This is where we put the Received SMS, yes above there's Recevied_SMS variable, we use a trick below
                                            //To concatenate the "char Recevied_SMS" to "String RSMS" which makes the "RSMS" contains the SMS received but as a String
                                            //The recevied SMS cannot be stored directly as String
  
  while(gprsSerial.available()>0){          //When SIM800L sends something to the Arduino... problably the SMS received... if something else it's not a problem
    Received_SMS = gprsSerial.read();       //"char Received_SMS" is now containing the full SMS received
    //Serial.print(Received_SMS);             //Show it on the serial monitor (optional)     
    RSMS.concat(Received_SMS);              //concatenate "char received_SMS" to RSMS which is "empty"
    for (int i=0; i<NUM_OF_CMD; i++) {      //And this is why we changed from char to String, it's to be able to use this function "indexOf"
      CMD[i] = RSMS.indexOf(Commands[i]);   //"indexOf function looks for the substring "x" within the String (here RSMS) and gives us its index or position
    }                                       //For example if found at the beginning it will give "0" after 1 character it will be "1"
  }                                         //If it's not found it will give "-1", so the variables are integers
}
//***************************************************************************************************************************//
void Action() {
  if      (CMD[0] != -1 || CMD[1] != -1) {
    loop_timer_1 = millis();
    M1on = true;
    if (CMD[0] != -1) M1cw = true;    // Motor 1 CW
    else              M1cw = false;   // Motor 1 CCW
  }
  else if (CMD[2] != -1 || CMD[3] != -1) {
    loop_timer_2 = millis();
    M2on = true;
    if (CMD[2] != -1) M2cw = true;    // Motor 2 CW
    else              M2cw = false;   // Motor 2 CCW
  }
  else if (CMD[4] != -1) digitalWrite(SWITCH_RELAY_1, LOW);    // Switch 1 on
  else if (CMD[5] != -1) digitalWrite(SWITCH_RELAY_2, LOW);    // Switch 2 on
  else if (CMD[6] != -1) digitalWrite(SWITCH_RELAY_3, LOW);    // Switch 3 on
  else if (CMD[7] != -1) digitalWrite(SWITCH_RELAY_4, LOW);    // Switch 4 on
  
  else if (CMD[8] != -1) digitalWrite(SWITCH_RELAY_1, HIGH);   // Switch 1 off
  else if (CMD[9] != -1) digitalWrite(SWITCH_RELAY_2, HIGH);   // Switch 2 off
  else if (CMD[10] != -1) digitalWrite(SWITCH_RELAY_3, HIGH);   // Switch 3 off
  else if (CMD[11] != -1) digitalWrite(SWITCH_RELAY_4, HIGH);   // Switch 4 off

  else if (CMD[12] != -1) {                                 // All Switch On
    digitalWrite(SWITCH_RELAY_1, LOW); delay(250);
    digitalWrite(SWITCH_RELAY_2, LOW); delay(250);
    digitalWrite(SWITCH_RELAY_3, LOW); delay(250);
    digitalWrite(SWITCH_RELAY_4, LOW);
  }
  else if (CMD[13] != -1) {                                 // All Switch Off
    digitalWrite(SWITCH_RELAY_1, HIGH); delay(250);
    digitalWrite(SWITCH_RELAY_2, HIGH); delay(250);
    digitalWrite(SWITCH_RELAY_3, HIGH); delay(250);
    digitalWrite(SWITCH_RELAY_4, HIGH);
  }
  else if (CMD[14] != -1) Status();                         // Status check
  else if (CMD[15] != -1) {                                 // Cmd List
    // SmsToSend = "\"M1Left\", \"M1Right\", \"M2Left\", \"M2Right\", \"Switch1on\", \"Switch2on\", \"Switch3on\", \"Switch4on\", \"Switch1off\", \"Switch2off\", \"Switch3off\", \"Switch4off\", \"AllSwitchOn\", \"AllSwitchOff\", \"Status\", \"Cmd\"";
    SmsToSend = "";
    for (int i = 0; i < NUM_OF_CMD; i++) {
      SmsToSend += "\"";
      SmsToSend += Commands[i];
      if (i < NUM_OF_CMD - 1) SmsToSend += "\", ";
      else                    SmsToSend += "\"";
    }
    //Serial.println(SmsToSend);
      SendTextMessage();
  }
  delay(250);
}
//***************************************************************************************************************************//
void SendTextMessage() {
  //Serial.println("Sending Text...");
  
  gprsSerial.print("AT+CMGF=1\r");                  // Set the shield to SMS mode
  delay(100);
  gprsSerial.print("AT+CMGS=\"972505577708\"\r");  
  delay(250);
  gprsSerial.print(SmsToSend); 
  delay(250);
  gprsSerial.print(" \r");                          //the content of the message
  delay(250);
  gprsSerial.print((char)26);                       //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(250);
  gprsSerial.println();
  //Serial.println("Text Sent.");
  delay(250);
  
  ReceiveMode();                                    //Come back to Receving SMS mode and wait for other SMS
}
//***************************************************************************************************************************//
void Serialcom() { //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
  delay(500);
  //while(Serial.available()) {
  //  gprsSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  //}
  while(gprsSerial.available()) {
    Serial.write(gprsSerial.read());//Forward what Software Serial received to Serial Port
  }
}
//***************************************************************************************************************************//
void ReceiveMode(){       //Set the SIM800L Receive mode
  gprsSerial.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  gprsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  Serialcom();
  gprsSerial.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
}
//***************************************************************************************************************************//
void Status() {
  SmsToSend = "";
  if (!digitalRead(MOTOR_RELAY_1)) SmsToSend += "M1 Left Moving, ";
  else SmsToSend += "No Left Moving M1, ";
  if (!digitalRead(MOTOR_RELAY_2)) SmsToSend += "M1 Right Moving, ";
  else SmsToSend += "No Right Moving M1, ";
  if (!digitalRead(MOTOR_RELAY_3)) SmsToSend += "M2 Left Moving, ";
  else SmsToSend += "No Left Moving M2, ";
  if (!digitalRead(MOTOR_RELAY_4)) SmsToSend += "M2 Right Moving, ";
  else SmsToSend += "No Right Moving M2, ";
  
  if (!digitalRead(SWITCH_RELAY_1)) SmsToSend += "SWITCH 1 On, ";
  else SmsToSend += "SWITCH 1 Off, ";
  if (!digitalRead(SWITCH_RELAY_2)) SmsToSend += "SWITCH 2 On, ";
  else SmsToSend += "SWITCH 2 Off, ";
  if (!digitalRead(SWITCH_RELAY_3)) SmsToSend += "SWITCH 3 On, ";
  else SmsToSend += "SWITCH 3 Off, ";
  if (!digitalRead(SWITCH_RELAY_4)) SmsToSend += "SWITCH 4 On, ";
  else SmsToSend += "SWITCH 4 Off, ";
  
  SendTextMessage();
}
//***************************************************************************************************************************//
void Direction_1() {
  if (loop_timer_1 + time_to_move_1 > millis()) {
    if (M1cw == true && microswish_1_state) {                                   // motor 1 CW
      digitalWrite(MOTOR_RELAY_1, HIGH);
      digitalWrite(MOTOR_RELAY_2, LOW);
    }
    else if (M1cw == true && !microswish_1_state) {
      digitalWrite(MOTOR_RELAY_1, HIGH);
      digitalWrite(MOTOR_RELAY_2, HIGH);
    }
    else if (M1cw == false && microswish_2_state) {                             // motor 1 CCW
      digitalWrite(MOTOR_RELAY_1, LOW);
      digitalWrite(MOTOR_RELAY_2, HIGH);
    }
    else if (M1cw == false && !microswish_2_state) {
      digitalWrite(MOTOR_RELAY_1, HIGH);
      digitalWrite(MOTOR_RELAY_2, HIGH);
    }
  }
  else {                                                  // motor 1 off
    digitalWrite(MOTOR_RELAY_1, HIGH);
    digitalWrite(MOTOR_RELAY_2, HIGH);
    M1on = false;
  }
}
//***************************************************************************************************************************//
void Direction_2() {
  if (loop_timer_2 + time_to_move_2 > millis()) {
    if (M2cw == true && microswish_3_state) {                                   // motor 2 CW
      digitalWrite(MOTOR_RELAY_3, HIGH);
      digitalWrite(MOTOR_RELAY_4, LOW);
    }
    else if (M2cw == true && !microswish_3_state) {
      digitalWrite(MOTOR_RELAY_3, HIGH);
      digitalWrite(MOTOR_RELAY_4, HIGH);
    }
    else if (M2cw == false && microswish_4_state) {                             // motor 2 CCW
      digitalWrite(MOTOR_RELAY_3, LOW);
      digitalWrite(MOTOR_RELAY_4, HIGH);
    }
    else if (M2cw == false && !microswish_4_state) {
      digitalWrite(MOTOR_RELAY_3, HIGH);
      digitalWrite(MOTOR_RELAY_4, HIGH);
    }
  }
  else {                                                  // motor 2 off
    digitalWrite(MOTOR_RELAY_3, HIGH);
    digitalWrite(MOTOR_RELAY_4, HIGH);
    M2on = false;
  }
}
//***************************************************************************************************************************//
