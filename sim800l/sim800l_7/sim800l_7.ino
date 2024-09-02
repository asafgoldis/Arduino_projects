//***************************************************************************************************************************//
//                                            Remote Control System V3.0
//***************************************************************************************************************************//
//      ביטול מנועים, חיבור ל8 ריליי בלבד, ביטול חיישן תנועה, ביטול לחצן

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
// Define IO
#define NUM_OF_CMD 21

#define PHONE_NUM 972505577708
String num = "972505577708";
#define RELAY_PIN_1 9
#define RELAY_PIN_2 10
#define RELAY_PIN_3 11
#define RELAY_PIN_4 12
#define RELAY_PIN_5 8
#define RELAY_PIN_6 7
#define RELAY_PIN_7 6
#define RELAY_PIN_8 5

//***************************************************************************************************************************//
SoftwareSerial gprsSerial(3,2);       // RX, TX

int CMD[NUM_OF_CMD];
String SmsToSend;
String Temp = "";
String Commands[NUM_OF_CMD] = {"Relay1on", "Relay2on", "Relay3on", "Relay4on", "Relay5on", "Relay6on", "Relay7on", "Relay8on", 
                               "Relay1off", "Relay2off", "Relay3off", "Relay4off", "Relay5off", "Relay6off", "Relay7off", "Relay8off", 
                               "AllRelayOn", "AllRelayOff", "Status", "Cmd", "Num"};
//***************************************************************************************************************************//
void setup() {
  gprsSerial.begin(9600); // GPRS shield baud rate
  Serial.begin(9600);
  
  RST_CMD();
  pinMode(RELAY_PIN_1, OUTPUT); digitalWrite(RELAY_PIN_1, HIGH);
  pinMode(RELAY_PIN_2, OUTPUT); digitalWrite(RELAY_PIN_2, HIGH);
  pinMode(RELAY_PIN_3, OUTPUT); digitalWrite(RELAY_PIN_3, HIGH);
  pinMode(RELAY_PIN_4, OUTPUT); digitalWrite(RELAY_PIN_4, HIGH);
  pinMode(RELAY_PIN_5, OUTPUT); digitalWrite(RELAY_PIN_5, HIGH);
  pinMode(RELAY_PIN_6, OUTPUT); digitalWrite(RELAY_PIN_6, HIGH);
  pinMode(RELAY_PIN_7, OUTPUT); digitalWrite(RELAY_PIN_7, HIGH);
  pinMode(RELAY_PIN_8, OUTPUT); digitalWrite(RELAY_PIN_8, HIGH);
  
  ReceiveMode();                  //Calling the function that puts the SIM800L moduleon receiving SMS mode
  //Serial.println("Start");
  delay(500);
}
//***************************************************************************************************************************//
void RST_CMD(){
  for (int i=0; i<NUM_OF_CMD; i++) {
    CMD[i] = -1;
  }
}
//***************************************************************************************************************************//
void loop() {
  if (gprsSerial.available()) {     // if the shield has something to say
    GetMsg();
    Action();
    ReceiveMode();
  }
  
  delay(200);
}
//***************************************************************************************************************************//
void GetMsg() {
  //Serial.println("GetMsg()");
  //Serial.println("available...");
  char Received_SMS = "";                        //Here we store the full received SMS (with phone sending number and date/time) as char
  String RSMS = "";                              //We add this new variable String type, and we put it in loop so everytime gets initialized
                                            //This is where we put the Received SMS, yes above there's Recevied_SMS variable, we use a trick below
                                            //To concatenate the "char Recevied_SMS" to "String RSMS" which makes the "RSMS" contains the SMS received but as a String
                                            //The recevied SMS cannot be stored directly as String
  
  while(gprsSerial.available()){          //When SIM800L sends something to the Arduino... problably the SMS received... if something else it's not a problem
    Received_SMS = gprsSerial.read();       //"char Received_SMS" is now containing the full SMS received
    //Serial.print(Received_SMS);           //Show it on the serial monitor (optional)     
    RSMS.concat(Received_SMS);              //concatenate "char received_SMS" to RSMS which is "empty"
    for (int i=0; i<NUM_OF_CMD; i++) {      //And this is why we changed from char to String, it's to be able to use this function "indexOf"
      CMD[i] = RSMS.indexOf(Commands[i]);   //"indexOf function looks for the substring "x" within the String (here RSMS) and gives us its index or position
    }                                       //For example if found at the beginning it will give "0" after 1 character it will be "1"
  }                                         //If it's not found it will give "-1", so the variables are integers
  
  
  num = ""; 
  for (int n=0; n<12; n++) {
    num += RSMS[10 + n];
  }

  
  //Serial.println("sending num = " + num);
  Serial.print("RSMS = "); Serial.println(RSMS);
  Serial.print("CMD[0] = "); Serial.println(CMD[0]); Serial.println(RSMS[CMD[0]]); Serial.println(RSMS[CMD[0]+Commands[0].length()]);
  Serial.print("RSMS.length() = "); Serial.println(RSMS.length());
  if (CMD[0] != -1 and RSMS.length()-CMD[0]-Commands[0].length() > 2) {
    String temp = "";
    int n = RSMS.length()-CMD[0]-Commands[0].length();
    for (int i=0; i<n; i++) {
      temp += RSMS[CMD[0]+Commands[0].length()+i];
    }
    Serial.print("temp = "); Serial.println(temp);
  }
}
//***************************************************************************************************************************//
void Action() {
  //Serial.println("Action()");
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

  else if (CMD[16] != -1) {                                 // All Relay On
    digitalWrite(RELAY_PIN_1, LOW); delay(250);
    digitalWrite(RELAY_PIN_2, LOW); delay(250);
    digitalWrite(RELAY_PIN_3, LOW); delay(250);
    digitalWrite(RELAY_PIN_4, LOW); delay(250);
    digitalWrite(RELAY_PIN_5, LOW); delay(250);
    digitalWrite(RELAY_PIN_6, LOW); delay(250);
    digitalWrite(RELAY_PIN_7, LOW); delay(250);
    digitalWrite(RELAY_PIN_8, LOW);
  }
  else if (CMD[17] != -1) {                                 // All Relay Off
    digitalWrite(RELAY_PIN_1, HIGH); delay(250);
    digitalWrite(RELAY_PIN_2, HIGH); delay(250);
    digitalWrite(RELAY_PIN_3, HIGH); delay(250);
    digitalWrite(RELAY_PIN_4, HIGH); delay(250);
    digitalWrite(RELAY_PIN_5, HIGH); delay(250);
    digitalWrite(RELAY_PIN_6, HIGH); delay(250);
    digitalWrite(RELAY_PIN_7, HIGH); delay(250);
    digitalWrite(RELAY_PIN_8, HIGH);
  }
  else if (CMD[18] != -1) Status();                         // Status check
  else if (CMD[19] != -1) {                                 // Cmd List
    SmsToSend = "\"Relay1on\", \"Relay2on\", \"Relay3on\", \"Relay4on\", \"Relay5on\", \"Relay6on\", \"Relay7on\", \"Relay8on\", \"Relay1off\", \"Relay2off\", \"Relay3off\", \"Relay4off\", \"Relay5off\", \"Relay6off\", \"Relay7off\", \"Relay8off\", \"AllRelayOn\", \"AllRelayOff\", \"Status\" \"Cmd\", \"Num\"";
    SendTextMessage();
  }
  else if (CMD[20] != -1) ChangeNum();                      // Change Number
  
  RST_CMD();
  delay(250);
}
//***************************************************************************************************************************//
void SendTextMessage() {
  //Serial.println("SendTextMessage()");
  //Serial.println("Sending Text...");
  
  gprsSerial.print("AT+CMGF=1\r");                  // Set the shield to SMS mode
  delay(50);
  //gprsSerial.print("AT+CMGS=\"972505577708\"\r");
  gprsSerial.print("AT+CMGS=\"" + num + "\"\r");
  delay(50);
  gprsSerial.print(SmsToSend);
  delay(50);
  gprsSerial.print(" \r");                          //the content of the message
  delay(50);
  gprsSerial.print((char)26);                       //the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(50);
  gprsSerial.println();
  //Serial.println("Text Sent.");
  delay(100);
  
  ReceiveMode();                                    //Come back to Receving SMS mode and wait for other SMS
}
//***************************************************************************************************************************//
void Serialcom() { //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
  delay(200);
  //while(Serial.available()) {
  //  gprsSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  //}
  while(gprsSerial.available()) {
    Serial.write(gprsSerial.read());//Forward what Software Serial received to Serial Port
  }
}
//***************************************************************************************************************************//
void ReceiveMode(){       //Set the SIM800L Receive mode
  //Serial.println("ReceiveMode()");
  gprsSerial.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  gprsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  Serialcom();
  gprsSerial.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
}
//***************************************************************************************************************************//
void Status() {
  //Serial.println("Status()");
  SmsToSend = "Relay Status is: ";
  if (!digitalRead(RELAY_PIN_1)) SmsToSend += "Relay 1 On";
  if (!digitalRead(RELAY_PIN_2)) SmsToSend += ", Relay 2 On";
  if (!digitalRead(RELAY_PIN_3)) SmsToSend += ", Relay 3 On";
  if (!digitalRead(RELAY_PIN_4)) SmsToSend += ", Relay 4 On";
  if (!digitalRead(RELAY_PIN_5)) SmsToSend += ", Relay 5 On";
  if (!digitalRead(RELAY_PIN_6)) SmsToSend += ", Relay 6 On";
  if (!digitalRead(RELAY_PIN_7)) SmsToSend += ", Relay 7 On";
  if (!digitalRead(RELAY_PIN_8)) SmsToSend += ", Relay 8 On";
  SmsToSend += ".";
  
  SendTextMessage();
}
//***************************************************************************************************************************//
void ChangeNum() {
  
  Serial.println("ChangeNum()");
  Serial.println("Temp = " + Temp);

}
//***************************************************************************************************************************//
