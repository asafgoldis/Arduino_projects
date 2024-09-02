//***************************************************************************************************************************//
//                                            Remote Control System V7.3
//***************************************************************************************************************************//
//      ביטול מנועים, חיבור ל8 ריליי בלבד, ביטול חיישן תנועה, ביטול לחצן
//  הכנסה של תזמון לפי שליחת מספר שניות
// ריסט לא עובד
//  תיקון באג שכאשר מפעילים יחידה לזמן קצוב ולפני הכיבוי מדליקים באופן קבוע היחידה נכבית בסוף הזמן

/*    IO Connaction:
// A0 - NOT CONNECTED
// A1 - NOT CONNECTED
// A2 - NOT CONNECTED
// A3 - NOT CONNECTED
// A4 - NOT CONNECTED
// A5 - NOT CONNECTED
// A6 - NOT CONNECTED
// A7 - NOT CONNECTED
// D0 - NOT CONNECTED 
// D1 - NOT CONNECTED
// D2 - RXD
// D3 - TXD
// D4 - RST_PIN
// D5 - Relay Unit 8
// D6 - Relay Unit 7
// D7 - Relay Unit 6
// D8 - Relay Unit 5
// D9 - Relay Unit 1
// D10 - Relay Unit 2
// D11 - Relay Unit 3
// D12 - Relay Unit 4
// D13 - LED
*/
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include <String.h>
//***************************************************************************************************************************//
String phoneNum = "972505577708";
const String SYS_NAME = "sys_2";
const String VERSION = "7.3";

#define NUM_OF_CMD 21
#define NUM_OF_RELAY 8
#define ON LOW
#define OFF HIGH

// Define IO
#define RST_PIN 4
#define RELAY_PIN_1 9
#define RELAY_PIN_2 10
#define RELAY_PIN_3 11
#define RELAY_PIN_4 12
#define RELAY_PIN_5 8
#define RELAY_PIN_6 7
#define RELAY_PIN_7 6
#define RELAY_PIN_8 5
#define LED_PIN 13
//***************************************************************************************************************************//
SoftwareSerial gprsSerial(3,2);               // RX, TX
const unsigned long MAXTIME = 4294967295;
const unsigned long DAY_21 = 1814400;         // 21 days: 21*24*60*60
const unsigned char RelayArray[NUM_OF_RELAY] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5, RELAY_PIN_6, RELAY_PIN_7, RELAY_PIN_8};
unsigned long RelayTimeArray[NUM_OF_RELAY];
unsigned long TimeInt = 0;
char CMD[NUM_OF_CMD];

const String Commands[NUM_OF_CMD] = { "R1on", "R2on", "R3on", "R4on", "R5on", "R6on", "R7on", "R8on", 
                                      "R1off", "R2off", "R3off", "R4off", "R5off", "R6off", "R7off", "R8off", 
                                      "Allon", "Alloff", "Status", "Cmd", "RST"};
//***************************************************************************************************************************//
void setup() {
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  for(int counter=0; counter<500; counter++) {                                //Create 500 loops, delay for 10 sec so the sim800 module get power
    if(counter%10 == 0)digitalWrite(LED_PIN, !digitalRead(LED_PIN));          //Change the state of the LED every 15 loops to make the LED blink fast
    delay(20);
  }
  
  Serial.begin(9600);
  gprsSerial.begin(9600);                                                     // GPRS shield baud rate

  for (int i=0; i<NUM_OF_RELAY; i++) {
    pinMode(RelayArray[i], OUTPUT);
    digitalWrite(RelayArray[i], OFF);
    RelayTimeArray[i] = MAXTIME;
  }
  RST_CMD();
  
  String SmsToSend = "\n\nSystem is up...\nSystem name: "; SmsToSend += SYS_NAME;
  SmsToSend += "\nSystem version: V"; SmsToSend += VERSION; SmsToSend += "\n\n";
  SmsToSend += "Write \"Cmd\" to see available commands.";
  Serial.println(SmsToSend);
  SendTextMessage(SmsToSend);
  
  ReceiveMode();                  //Calling the function that puts the SIM800L moduleon receiving SMS mode
  delay(500);

  //Serial.println(""); Serial.println(SYS_NAME + " Ready...");
}
//***************************************************************************************************************************//
void loop() {
  //Serial.print("millis() = "); Serial.println(millis());
  if (gprsSerial.available()) {     // if the shield has something to say
    Action(GetMsg());
    ReceiveMode();
  }
  //PrintRelayStatus();
  
  testTime();
  delay(100);
  //Serial.print("millis()/1000 = "); Serial.println(millis()/1000);
  if (millis()/1000 > DAY_21)
    Reboot();
}
//***************************************************************************************************************************//
int GetMsg() {
  String textMessage;
  int cmdNum = -1;
  TimeInt = 0;
  
  textMessage = gprsSerial.readString();
  Serial.print(textMessage);
  delay(10);
  
  for (int i=0; i<NUM_OF_CMD; i++) {
    CMD[i] = textMessage.indexOf(Commands[i]);
    if (CMD[i] != -1) cmdNum = i;
  }
  
  phoneNum = ""; 
  for (int n=0; n<12; n++)
    phoneNum += textMessage[10 + n];
  // Serial.println("phoneNum = " + phoneNum);

  if (cmdNum != -1 and cmdNum<NUM_OF_RELAY) {                                   //  if you need to start relay on
    int n = textMessage.length()-CMD[cmdNum]-Commands[cmdNum].length();         //  get the length of the remaining string
    if (n > 2) {                                                                //  if the msg long enough and contain more char
      String Time = "";
      for (int i=0; i<n; i++)
        Time += textMessage[CMD[cmdNum]+Commands[cmdNum].length()+i];           //  get the string
      
      TimeInt = Time.toInt();
      // Serial.print("Time = "); Serial.print(Time); Serial.print(", TimeInt = "); Serial.println(TimeInt);
      if (TimeInt > DAY_21) {
        SendTextMessage("21 day is the maximum time....(1814400 sec)\n Relay status has no change");
        TimeInt = 0;
        cmdNum = -1;
      }
    }
  }
  
  return cmdNum;
}
//***************************************************************************************************************************//
void Action(int cmdNum) {
  if (0 <= cmdNum and cmdNum < NUM_OF_RELAY) {                       // Relay on (cmd 0-7)
    digitalWrite(RelayArray[cmdNum], ON);
    if (TimeInt > 0) {
      unsigned long currentSec = millis()/1000;
      RelayTimeArray[cmdNum] = currentSec + TimeInt;
    }
    else
      RelayTimeArray[cmdNum] = MAXTIME;
  }
  else if (NUM_OF_RELAY <= cmdNum and cmdNum < NUM_OF_RELAY*2) {     // Relay off (cmd 9-15)
    digitalWrite(RelayArray[cmdNum-NUM_OF_RELAY], OFF);
    RelayTimeArray[cmdNum-NUM_OF_RELAY] = MAXTIME;
  }
  
  else if (cmdNum == 16) ChangeAllRelay(ON);                        // All Relay On
  else if (cmdNum == 17) ChangeAllRelay(OFF);                       // All Relay Off
  
  else if (cmdNum == 18) SendTextMessage(Status());                 // Status check
  else if (cmdNum == 19) PrintCmd();                                // Cmd List
  else if (cmdNum == 20) Reboot();                                  // Soft reset
  
  RST_CMD();
}
//***************************************************************************************************************************//
void ReceiveMode(){                         //Set the SIM800L Receive mode
  gprsSerial.println("AT");                 //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  gprsSerial.println("AT+CMGF=1");          // Configuring TEXT mode
  Serialcom();
  gprsSerial.println("AT+CNMI=2,2,0,0,0");  //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
}
//***************************************************************************************************************************//
void Serialcom() {                          //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
  delay(25);
  //while(Serial.available()) {
  //  gprsSerial.write(Serial.read());      //Forward what Serial received to Software Serial Port
  //}
  while(gprsSerial.available()) {
    Serial.write(gprsSerial.read());        //Forward what Software Serial received to Serial Port
  }
}
//***************************************************************************************************************************//
void testTime() {
  unsigned long currentSec = millis()/1000;
  for (int i=0; i<NUM_OF_RELAY; i++) {
    if (currentSec >= RelayTimeArray[i]) {
      RelayTimeArray[i] = MAXTIME;
      digitalWrite(RelayArray[i], OFF);   // Relay i off
    }
  }
}
//***************************************************************************************************************************//
void ChangeAllRelay(bool st) {
  for (int i=0; i<NUM_OF_RELAY; i++) {
    digitalWrite(RelayArray[i], st);
    RelayTimeArray[i] = MAXTIME;
    delay(125);
  }
}
//***************************************************************************************************************************//
void RST_CMD(){
  for (int i=0; i<NUM_OF_CMD; i++)
    CMD[i] = -1;
}
//***************************************************************************************************************************//
void SendTextMessage(String SMS) {
  gprsSerial.print("AT+CMGF=1\r");                  // Set the shield to SMS mode
  delay(25);
  gprsSerial.print("AT+CMGS=\"" + phoneNum + "\"\r");
  delay(25);
  gprsSerial.print(SMS);
  delay(25);
  gprsSerial.print(" \r");                          //the content of the message
  delay(25);
  gprsSerial.write(26);                             //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(25);
  
  ReceiveMode();                                    //Come back to Receving SMS mode and wait for other SMS
}
//***************************************************************************************************************************//
String Status() {
  String SmsToSend = "Relay Status ("; SmsToSend += SYS_NAME; SmsToSend += ") is: \n";
  bool allOffFlag = true;
  for (int i=0; i<NUM_OF_RELAY; i++) {
    SmsToSend += "Relay"; SmsToSend += i+1;
    if (!digitalRead(RelayArray[i])) {        // if relay on
      allOffFlag = false;
      SmsToSend += "on";
      if (RelayTimeArray[i] != MAXTIME) {
        SmsToSend += "(";
        SmsToSend += ConvTime(i);
        SmsToSend += ")";
      }
      SmsToSend += "\n";
    }
    else                                      // if relay off
      SmsToSend += "off\n";
  }
  
  if (allOffFlag)
    SmsToSend = "All relay off";
    
  return SmsToSend;
}
//***************************************************************************************************************************//
String ConvTime(int relayNum) {
  String StringTime = "";
  unsigned long currentSec = millis()/1000;
  unsigned long numTime = RelayTimeArray[relayNum]-currentSec;
  unsigned int Sec, Min, Hour, Day;
  
  Sec = numTime%60;
  numTime -= Sec;
  numTime /= 60;
  
  Min = numTime%60;
  numTime -= Min;
  numTime /= 60;
  
  Hour = numTime%24;
  numTime -= Hour;
  Day = numTime/24;
  
  if (Day<10) StringTime += "0";
  StringTime += Day; StringTime += "d:";
  
  if (Hour<10) StringTime += "0";
  StringTime += Hour; StringTime += "h:";

  if (Min<10) StringTime += "0";
  StringTime += Min; StringTime += "m:";

  if (Sec<10) StringTime += "0";
  StringTime += Sec; StringTime += "s";
  
  return StringTime;
}
//***************************************************************************************************************************//
void PrintCmd() {
  String SmsToSend = "Available Commands: ";
  for (int i=0; i<NUM_OF_CMD; i++) {
    SmsToSend += "\""; SmsToSend += Commands[i]; SmsToSend += "\"";     // SmsToSend += "Commands[i]"
    if (i<NUM_OF_CMD-1) SmsToSend += ", ";
  }
  SmsToSend += ".";
  SendTextMessage(SmsToSend);
}
//***************************************************************************************************************************//
void PrintRelayStatus() {
  String RelayStatus = "";
  for (int i=0; i<NUM_OF_RELAY; i++) {
    RelayStatus += "R"; RelayStatus += i+1;
    if (!digitalRead(RelayArray[i])) {              // if relay on
      RelayStatus += "on(";
      if (RelayTimeArray[i] == MAXTIME)
        RelayStatus += "inf), ";
      else {
        RelayStatus += ConvTime(i);
        RelayStatus += "), ";
      }
    }
    else                                          // if relay off
      RelayStatus += "off, ";
  }
  
  Serial.println(RelayStatus);
}
//***************************************************************************************************************************//
void Reboot() {                                                               // soft reset
  // NPN transistor: base to D4 through 330 ohm resistor, emitter(arrow) to GND, collector to RST
  String SmsToSend = "Rebooting.....in 5 sec\n\n";
  SmsToSend += Status();
  SendTextMessage(SmsToSend);
  //Serial.println(SmsToSend);
  for(int counter=0; counter<500; counter++) {                                //Create 500 loops, delay for 5 sec so the sim800 module get power
    if(counter%10 == 0)digitalWrite(LED_PIN, !digitalRead(LED_PIN));          //Change the state of the LED every 15 loops to make the LED blink fast
    delay(10);
  }
  digitalWrite(RST_PIN, HIGH);

  // code should never get to here
  //Serial.println("Reboot failed");
}
//***************************************************************************************************************************//
