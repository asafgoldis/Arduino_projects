//***************************************************************************************************************************//
//                                            Remote Control System V7.1
//***************************************************************************************************************************//
//      ביטול מנועים, חיבור ל8 ריליי בלבד, ביטול חיישן תנועה, ביטול לחצן
//  הכנסה של תזמון לפי שליחת מספר שניות
// ריסט לא עובד

//    IO Connaction:
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
// D4 - NOT CONNECTED
// D5 - Relay Unit 8
// D6 - Relay Unit 7
// D7 - Relay Unit 6
// D8 - Relay Unit 5
// D9 - Relay Unit 1
// D10 - Relay Unit 2
// D11 - Relay Unit 3
// D12 - Relay Unit 4
// D13 - NOT CONNECTED
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include <String.h>
//***************************************************************************************************************************//
String phoneNum = "972505577708";
const String SYS_NAME = "sys_2";
const String VERSION = "7.1";

#define NUM_OF_CMD 21
#define NUM_OF_RELAY 8

// Define IO
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
SoftwareSerial gprsSerial(3,2);       // RX, TX
const unsigned long MAXTIME = 4294967295;
const unsigned int RelayArray[NUM_OF_RELAY] = {RELAY_PIN_1, RELAY_PIN_2, RELAY_PIN_3, RELAY_PIN_4, RELAY_PIN_5, RELAY_PIN_6, RELAY_PIN_7, RELAY_PIN_8};
unsigned long RelayTimeArray[NUM_OF_RELAY];
int CMD[NUM_OF_CMD];
//int counter = 0;

const String Commands[NUM_OF_CMD] = { "R1on", "R2on", "R3on", "R4on", "R5on", "R6on", "R7on", "R8on", 
                                      "R1off", "R2off", "R3off", "R4off", "R5off", "R6off", "R7off", "R8off", 
                                      "Allon", "Alloff", "Status", "Cmd", "RST"};
//***************************************************************************************************************************//
void setup() {
  pinMode(LED_PIN, OUTPUT); 
  for (int i=0; i<NUM_OF_RELAY; i++) {
    pinMode(RelayArray[i], OUTPUT);
    digitalWrite(RelayArray[i], HIGH);
    RelayTimeArray[i] = MAXTIME;
  }
  RST_CMD();

  for(int counter = 0; counter < 500; counter++){                             //Create 500 loops, delay for 10 sec so the sim800 module get power
    delay(20);
    if(counter % 10 == 0)digitalWrite(LED_PIN, !digitalRead(LED_PIN));        //Change the state of the LED every 15 loops to make the LED blink fast
  }

  Serial.begin(9600);
  gprsSerial.begin(9600);         // GPRS shield baud rate
  
  String SmsToSend = "System is up...\nSys name: ";
  SmsToSend += SYS_NAME;
  SmsToSend += ".\nSys version: V";
  SmsToSend += VERSION;
  SmsToSend += ".\n\n";
  SmsToSend += "Write \"Cmd\" to see available commands.";
  Serial.println(SmsToSend);
  SendTextMessage(SmsToSend);
  
  ReceiveMode();                  //Calling the function that puts the SIM800L moduleon receiving SMS mode
  delay(500);

  Serial.println(""); Serial.println(SYS_NAME + " Ready...");
}
//***************************************************************************************************************************//
void loop() {
  //Serial.println(millis());
  if (gprsSerial.available()) {     // if the shield has something to say
    GetMsg();
    Action();
    ReceiveMode();
  }
  testTime();
  
  delay(100);
}
//***************************************************************************************************************************//
void GetMsg() {
  String textMessage;
  int cmdNum = -1;
  
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
  
  //Serial.println("phoneNum = " + phoneNum);
  //Serial.print("length() = "); Serial.println(textMessage.length());
  
  if (cmdNum != -1 and cmdNum<NUM_OF_RELAY) {
    if (textMessage.length()-CMD[cmdNum]-Commands[cmdNum].length() > 2) {
      String time = "";
      int n = textMessage.length()-CMD[cmdNum]-Commands[cmdNum].length();
      for (int i=0; i<n; i++)
        time += textMessage[CMD[cmdNum]+Commands[cmdNum].length()+i];
      
      //Serial.print("time = "); Serial.println(time);
      //Serial.print("time = "); Serial.println(time.toInt());
      if (time.toInt() > 0)
        RelayTimeArray[cmdNum] = millis() + 1000*time.toInt();
    }
  }

}
//***************************************************************************************************************************//
void Action() {
  if      (CMD[0] != -1) digitalWrite(RELAY_PIN_1, LOW);    // Relay 1 on
  else if (CMD[1] != -1) digitalWrite(RELAY_PIN_2, LOW);    // Relay 2 on
  else if (CMD[2] != -1) digitalWrite(RELAY_PIN_3, LOW);    // Relay 3 on
  else if (CMD[3] != -1) digitalWrite(RELAY_PIN_4, LOW);    // Relay 4 on
  else if (CMD[4] != -1) digitalWrite(RELAY_PIN_5, LOW);    // Relay 5 on
  else if (CMD[5] != -1) digitalWrite(RELAY_PIN_6, LOW);    // Relay 6 on
  else if (CMD[6] != -1) digitalWrite(RELAY_PIN_7, LOW);    // Relay 7 on
  else if (CMD[7] != -1) digitalWrite(RELAY_PIN_8, LOW);    // Relay 8 on
  
  else if (CMD[8] != -1) digitalWrite(RELAY_PIN_1, HIGH);   // Relay 1 off
  else if (CMD[9] != -1) digitalWrite(RELAY_PIN_2, HIGH);   // Relay 2 off
  else if (CMD[10] != -1) digitalWrite(RELAY_PIN_3, HIGH);   // Relay 3 off
  else if (CMD[11] != -1) digitalWrite(RELAY_PIN_4, HIGH);   // Relay 4 off 
  else if (CMD[12] != -1) digitalWrite(RELAY_PIN_5, HIGH);   // Relay 5 off
  else if (CMD[13] != -1) digitalWrite(RELAY_PIN_6, HIGH);   // Relay 6 off
  else if (CMD[14] != -1) digitalWrite(RELAY_PIN_7, HIGH);   // Relay 7 off
  else if (CMD[15] != -1) digitalWrite(RELAY_PIN_8, HIGH);   // Relay 8 off

  else if (CMD[16] != -1) ChangeAllRelay(LOW);               // All Relay On
  else if (CMD[17] != -1) ChangeAllRelay(HIGH);              // All Relay Off

  else if (CMD[18] != -1) Status();                         // Status check
  else if (CMD[19] != -1) Cmd();                            // Cmd List
  else if (CMD[20] != -1) Reboot();                         // Soft reset
  
  RST_CMD();
  //delay(250);
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
void Serialcom() {        //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
  delay(25);
  //while(Serial.available()) {
  //  gprsSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  //}
  while(gprsSerial.available()) {
    Serial.write(gprsSerial.read());//Forward what Software Serial received to Serial Port
  }
}
//***************************************************************************************************************************//
void testTime() {
  unsigned long currentTime = millis();
  for (int i=0; i<NUM_OF_RELAY; i++) {
    if (currentTime >= RelayTimeArray[i]) {
      RelayTimeArray[i] = MAXTIME;
      digitalWrite(RelayArray[i], HIGH);   // Relay i off
    }
  }
}
//***************************************************************************************************************************//
void ChangeAllRelay(bool st) {
  for (int i=0; i<NUM_OF_RELAY; i++) {
    digitalWrite(RelayArray[i], st);
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
  //Serial.println("Sending Text...");
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
void Status() {
  String SmsToSend = "Relay Status (";
  SmsToSend += SYS_NAME;
  SmsToSend += ") is: \n";
  bool allOffFlag = true;
  for (int i=0; i<NUM_OF_RELAY; i++) {
    if (!digitalRead(RelayArray[i])) {
      allOffFlag = false;
      if (i != 0) SmsToSend += ", ";
      SmsToSend += "Relay ";
      SmsToSend += i+1;
      SmsToSend += " On";
    }
  }
  SmsToSend += ".";
  if (allOffFlag)
    SmsToSend = "All relay off";
    
  SendTextMessage(SmsToSend);
}
//***************************************************************************************************************************//
void Cmd() {
  //Serial.println("Cmd()");
  String SmsToSend = "Available Commands: ";
  for (int i=0; i<NUM_OF_CMD; i++) {
    SmsToSend += "\"";
    SmsToSend += Commands[i];
    SmsToSend += "\"";
    if (i<NUM_OF_CMD-1) SmsToSend += ", ";
  }
  SmsToSend += ".";
  SendTextMessage(SmsToSend);
}
//***************************************************************************************************************************//
void Reboot() {
  Serial.println("Rebooting...");
  
}
//***************************************************************************************************************************//
