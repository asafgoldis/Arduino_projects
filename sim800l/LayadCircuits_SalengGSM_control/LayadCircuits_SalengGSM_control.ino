/***************************************************************************
 Example Code to control via SMS the builtin LED onboard the Arduino board wired to pin 13.
 To execute a command, the user sends an SMS containing one of the two keywords available.
 Open the serial monitor and set the baud rate to 9600 to view debug messages.
 
 This is an Arduino example sketch designed specifically to work with the Layad Circuits' Saleng GSM Shield.
 
 This software is free provided that this notice is not removed and proper attribution 
 is accorded to Layad Circuits and its Author(s).
 Layad Circuits invests resources in producing free software. By purchasing Layad Circuits'
 products or utilizing its services, you support the continuing development of free software
 for all.
  
 Author(s): C.D.Malecdan for Layad Circuits Electronics Engineering
 Revision: 1.0 - 2018/06/20 - initial creation
 Layad Circuits Electronics Engineering Supplies and Services
 B314 Lopez Bldg., Session Rd. cor. Assumption Rd., Baguio City, Philippines
 www.layadcircuits.com
 general: info@layadcircuits.com
 sales: sales@layadcircuits.com
 +63-916-442-8565
 ***************************************************************************/
 //***************************************************************************************************************************//
//                                            Remote Control System V1.0
//***************************************************************************************************************************//

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
// D2 - TX
// D3 - RX
// D4 - NOT CONNECTED
// D5 - Relay Unit 1
// D6 - Relay Unit 2
// D7 - Relay Unit 3
// D8 - Relay Unit 4
// D9 - Button input
// D10 - PIR input
// D11 - NOT CONNECTED
// D12 - PIR led (220 Ohm resistor)
// D13 - Led
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include <LayadCircuits_SalengGSM.h>
#include <String.h>
//***************************************************************************************************************************//
// Define IO
#define NUM_OF_CMD 16

#define PHONE_NUM +972505577708
#define RELAY_PIN_4 5
#define RELAY_PIN_3 6
#define RELAY_PIN_2 7
#define RELAY_PIN_1 8
#define BUTTON_PIN 9
#define PIR_PIN 10                    // the pin that the PIR sensor is atteched to
#define PIR_LED_PIN 11
#define LED_PIN 13

// You may edit these keywords to your liking. Do note the following pointers
// we recommend using at least 4 characters
// avoid pure numeric characters
// shorter keywords must not match characters within a longer keyword (e.g. dont use SALE if you already have a keyword called SALENG)
// do not use the following: OK, ERROR, CMT, CMTI, Ready, ready
#define KEYWORD1 "Relay1on"
#define KEYWORD2 "Relay2on"
#define KEYWORD3 "Relay3on"
#define KEYWORD4 "Relay4on"
#define KEYWORD5 "Relay1off"
#define KEYWORD6 "Relay2off"
#define KEYWORD7 "Relay3off"
#define KEYWORD8 "Relay4off"
#define KEYWORD9 "LedOn"
#define KEYWORD10 "LedOff"
#define KEYWORD11 "AllRelayOn"
#define KEYWORD12 "AllRelayOff"
#define KEYWORD13 "PirEnable"
#define KEYWORD14 "PirDisable"
#define KEYWORD15 "Status"
#define KEYWORD16 "Cmd"


SoftwareSerial gsmSerial(3,2);         // RX, TX
LayadCircuits_SalengGSM salengGSM = LayadCircuits_SalengGSM(&gsmSerial);



int CMD[NUM_OF_CMD];
String SmsToSend;

String Commands[NUM_OF_CMD] = {"Relay1on", "Relay2on", "Relay3on", "Relay4on", 
                               "Relay1off", "Relay2off", "Relay3off", "Relay4off", 
                               "LedOn", "LedOff", "AllRelayOn", "AllRelayOff", "PirEnable", "PirDisable", "Status", "Cmd"};
/*
char Commands[NUM_OF_CMD] = {'Relay1on', 'Relay2on', 'Relay3on', 'Relay4on', 
                               'Relay1off', 'Relay2off', 'Relay3off', 'Relay4off', 
                               'LedOn', 'LedOff', 'AllRelayOn', 'AllRelayOff', 'PirEnable', 'PirDisable', 'Status', 'Cmd'};
*/                            
char Received_SMS;                    //Here we store the full received SMS (with phone sending number and date/time) as char
bool buttonState = LOW;
bool PIR_state = LOW;                 // by default, no motion detected
bool PIR_val = LOW;                   // variable to store the sensor status (value)
bool PIR_Active = false;

void setup() {

  salengGSM.begin(9600); // this is the default baud rate
  Serial.begin(9600);

  
  for (int i=0; i<NUM_OF_CMD; i++) {
    CMD[i] = -1;
  }
  pinMode(RELAY_PIN_1, OUTPUT); digitalWrite(RELAY_PIN_1, HIGH);
  pinMode(RELAY_PIN_2, OUTPUT); digitalWrite(RELAY_PIN_2, HIGH);
  pinMode(RELAY_PIN_3, OUTPUT); digitalWrite(RELAY_PIN_3, HIGH);
  pinMode(RELAY_PIN_4, OUTPUT); digitalWrite(RELAY_PIN_4, HIGH);
  pinMode(LED_PIN    , OUTPUT); digitalWrite(LED_PIN    , LOW);                                  
  pinMode(BUTTON_PIN , INPUT_PULLUP );
  pinMode(PIR_PIN    , INPUT );
  
  
  Serial.print(F("Preparing Saleng GSM Shield.Pls wait for 5 seconds..."));
  delay(5000); // allow 10 seconds for modem to boot up and register
  salengGSM.initSalengGSM();
  Serial.println(F("Done"));
}

void loop() {
  salengGSM.smsMachine(); // we need to pass here as fast as we can. this allows for non-blocking SMS transmission
  
  if(salengGSM.isSMSavailable()) { // we also need to pass here as frequent as possible to check for incoming messages
    GetMsg();
    //Action();
  }

  buttonState = digitalRead(BUTTON_PIN);
  if (!buttonState) {
    delay(300);
    SmsToSend = "buttonPress";
    SendTextMessage(); 
  }

  if (PIR_Active) ReadPIR();
  else digitalWrite(PIR_LED_PIN, LOW);       // turn LED OFF
  
}

void GetMsg () {
  
  salengGSM.readSMS(); // updates the read flag

  //Serial.print("Sender=");
  //Serial.println(salengGSM.smsSender);
  Serial.print("Whole Message=");
  Serial.println(salengGSM.smsRxMsg); // if we receive an SMS, print the contents of the receive buffer
  
  if     (strstr(salengGSM.smsRxMsg, KEYWORD1))   digitalWrite(RELAY_PIN_1, LOW);
  else if(strstr(salengGSM.smsRxMsg, KEYWORD2))   digitalWrite(RELAY_PIN_2, LOW);
  else if(strstr(salengGSM.smsRxMsg, KEYWORD3))   digitalWrite(RELAY_PIN_3, LOW);
  else if(strstr(salengGSM.smsRxMsg, KEYWORD4))   digitalWrite(RELAY_PIN_4, LOW);
  
  else if(strstr(salengGSM.smsRxMsg, KEYWORD5))   digitalWrite(RELAY_PIN_1, HIGH);
  else if(strstr(salengGSM.smsRxMsg, KEYWORD6))   digitalWrite(RELAY_PIN_2, HIGH);
  else if(strstr(salengGSM.smsRxMsg, KEYWORD7))   digitalWrite(RELAY_PIN_3, HIGH);
  else if(strstr(salengGSM.smsRxMsg, KEYWORD8))   digitalWrite(RELAY_PIN_4, HIGH);
  
  else if(strstr(salengGSM.smsRxMsg, KEYWORD9))   digitalWrite(LED_PIN, HIGH);
  else if(strstr(salengGSM.smsRxMsg, KEYWORD10))   digitalWrite(LED_PIN, LOW);
  
  else if (strstr(salengGSM.smsRxMsg, KEYWORD11)) {
    digitalWrite(RELAY_PIN_1, LOW); delay(50);
    digitalWrite(RELAY_PIN_2, LOW); delay(50);
    digitalWrite(RELAY_PIN_3, LOW); delay(50);
    digitalWrite(RELAY_PIN_4, LOW);
  }
  else if (strstr(salengGSM.smsRxMsg, KEYWORD12)) {
    digitalWrite(RELAY_PIN_1, HIGH); delay(50);
    digitalWrite(RELAY_PIN_2, HIGH); delay(50);
    digitalWrite(RELAY_PIN_3, HIGH); delay(50);
    digitalWrite(RELAY_PIN_4, HIGH);
  }
  else if (strstr(salengGSM.smsRxMsg, KEYWORD13)) PIR_Active = true;
  else if (strstr(salengGSM.smsRxMsg, KEYWORD14)) PIR_Active = false;
  else if (strstr(salengGSM.smsRxMsg, KEYWORD15)) Status();
  else if (strstr(salengGSM.smsRxMsg, KEYWORD16)) {
    SmsToSend = "\"Relay1on\", \"Relay2on\", \"Relay3on\", \"Relay4on\", \"Relay1off\", \"Relay2off\", \"Relay3off\", \"Relay4off\", \"LedOn\", \"LedOff\", \"AllRelayOn\", \"AllRelayOff\", \"PirEnable\", \"PirDisable\", \"Status\" \"Cmd\"";
    SendTextMessage();
  }
  
   
}

void Action () {
  /*
     if(strstr(salengGSM.smsRxMsg,KEYWORD1)) {// check if keyword 1 was received
      // do something here
      salengGSM.sendSMS("972505577708","Led On");
       digitalWrite(LED_BUILTIN,HIGH);
     }
     else if(strstr(salengGSM.smsRxMsg,KEYWORD2)) { // check if keyword 2 was received
      // do something here
      salengGSM.sendSMS("972505577708","Led Off");
       digitalWrite(LED_BUILTIN,LOW);
     }

*/
/*
  if(strstr(salengGSM.smsRxMsg, CMD[0])) digitalWrite(RELAY_PIN_1, LOW);
  else if(strstr(salengGSM.smsRxMsg, CMD[1]))   digitalWrite(RELAY_PIN_2, LOW);
  else if(strstr(salengGSM.smsRxMsg, CMD[2]))   digitalWrite(RELAY_PIN_3, LOW);
  else if(strstr(salengGSM.smsRxMsg, CMD[3]))   digitalWrite(RELAY_PIN_4, LOW);
  
  else if(strstr(salengGSM.smsRxMsg, CMD[4]))   digitalWrite(RELAY_PIN_1, HIGH);
  else if(strstr(salengGSM.smsRxMsg, CMD[5]))   digitalWrite(RELAY_PIN_2, HIGH);
  else if(strstr(salengGSM.smsRxMsg, CMD[6]))   digitalWrite(RELAY_PIN_3, HIGH);
  else if(strstr(salengGSM.smsRxMsg, CMD[7]))   digitalWrite(RELAY_PIN_4, HIGH);
  
  else if(strstr(salengGSM.smsRxMsg, CMD[8]))   digitalWrite(LED_PIN, HIGH);
  else if(strstr(salengGSM.smsRxMsg, CMD[9]))   digitalWrite(LED_PIN, LOW);
     
  else if (strstr(salengGSM.smsRxMsg, CMD[10])) {
    digitalWrite(RELAY_PIN_1, LOW); delay(250);
    digitalWrite(RELAY_PIN_2, LOW); delay(250);
    digitalWrite(RELAY_PIN_3, LOW); delay(250);
    digitalWrite(RELAY_PIN_4, LOW);
  }
  else if (strstr(salengGSM.smsRxMsg, CMD[11])) {
    digitalWrite(RELAY_PIN_1, HIGH); delay(250);
    digitalWrite(RELAY_PIN_2, HIGH); delay(250);
    digitalWrite(RELAY_PIN_3, HIGH); delay(250);
    digitalWrite(RELAY_PIN_4, HIGH);
  }
  else if (strstr(salengGSM.smsRxMsg, CMD[12])) PIR_Active = true;
  else if (strstr(salengGSM.smsRxMsg, CMD[13])) PIR_Active = false;
  else if (strstr(salengGSM.smsRxMsg, CMD[14])) Status();
  else if (strstr(salengGSM.smsRxMsg, CMD[15])) {
    SmsToSend = "\"Relay1on\", \"Relay2on\", \"Relay3on\", \"Relay4on\", \"Relay1off\", \"Relay2off\", \"Relay3off\", \"Relay4off\", \"LedOn\", \"LedOff\", \"AllRelayOn\", \"AllRelayOff\", \"PirEnable\", \"PirDisable\", \"Status\" \"Cmd\"";
    SendTextMessage();
  }
  */

  if      (CMD[0] != -1) digitalWrite(RELAY_PIN_1, LOW);
  else if (CMD[1] != -1) digitalWrite(RELAY_PIN_2, LOW);
  else if (CMD[2] != -1) digitalWrite(RELAY_PIN_3, LOW);
  else if (CMD[3] != -1) digitalWrite(RELAY_PIN_4, LOW);
  
  else if (CMD[4] != -1) digitalWrite(RELAY_PIN_1, HIGH);
  else if (CMD[5] != -1) digitalWrite(RELAY_PIN_2, HIGH);
  else if (CMD[6] != -1) digitalWrite(RELAY_PIN_3, HIGH);
  else if (CMD[7] != -1) digitalWrite(RELAY_PIN_4, HIGH);
  
  else if (CMD[8] != -1) digitalWrite(LED_PIN, HIGH);
  else if (CMD[9] != -1) digitalWrite(LED_PIN, LOW);

  else if (CMD[10] != -1) {
    digitalWrite(RELAY_PIN_1, LOW); delay(250);
    digitalWrite(RELAY_PIN_2, LOW); delay(250);
    digitalWrite(RELAY_PIN_3, LOW); delay(250);
    digitalWrite(RELAY_PIN_4, LOW);
  }
  else if (CMD[11] != -1) {
    digitalWrite(RELAY_PIN_1, HIGH); delay(250);
    digitalWrite(RELAY_PIN_2, HIGH); delay(250);
    digitalWrite(RELAY_PIN_3, HIGH); delay(250);
    digitalWrite(RELAY_PIN_4, HIGH);
  }
  else if (CMD[12] != -1) PIR_Active = true;
  else if (CMD[13] != -1) PIR_Active = false;
  else if (CMD[14] != -1) Status();
  else if (CMD[15] != -1) {
    SmsToSend = "\"Relay1on\", \"Relay2on\", \"Relay3on\", \"Relay4on\", \"Relay1off\", \"Relay2off\", \"Relay3off\", \"Relay4off\", \"LedOn\", \"LedOff\", \"AllRelayOn\", \"AllRelayOff\", \"PirEnable\", \"PirDisable\", \"Status\" \"Cmd\"";
    SendTextMessage();
  }
  delay(250);
  
}

void SendTextMessage() {
  
  Serial.print("Sending Text: "); Serial.println(SmsToSend);
  
  salengGSM.sendSMS("972505577708",SmsToSend.c_str());
  //salengGSM.sendSMS("972505577708",SmsToSend);
  
}



void ReadPIR() {
  PIR_val = digitalRead(PIR_PIN);         // read sensor value
  if (PIR_val) {                          // check if the sensor is HIGH
    digitalWrite(PIR_LED_PIN, HIGH);      // turn LED ON
    delay(50);
    if (!PIR_state) {
      SmsToSend = "Motion detected!";
      //Serial.println(SmsToSend);
      //SendTextMessage();
      PIR_state = HIGH;                   // update variable state to HIGH
    }
  }
  else {
    digitalWrite(PIR_LED_PIN, LOW);       // turn LED OFF
    delay(50);
    if (PIR_state) {
      SmsToSend = "Motion stopped!";
      //Serial.println(SmsToSend);
      //SendTextMessage();
      PIR_state = LOW;                    // update variable state to LOW
    }
  }
}
//***************************************************************************************************************************//
void Status() {
  SmsToSend = "";
  if (!digitalRead(RELAY_PIN_1)) SmsToSend += "Relay 1 On, ";
  else SmsToSend += "Relay 1 Off, ";
  if (!digitalRead(RELAY_PIN_2)) SmsToSend += "Relay 2 On, ";
  else SmsToSend += "Relay 2 Off, ";
  if (!digitalRead(RELAY_PIN_3)) SmsToSend += "Relay 3 On, ";
  else SmsToSend += "Relay 3 Off, ";
  if (!digitalRead(RELAY_PIN_4)) SmsToSend += "Relay 4 On, ";
  else SmsToSend += "Relay 4 Off, ";
  if (digitalRead(LED_PIN)) SmsToSend += "Led On, ";
  else SmsToSend += "Led Off, ";
  if (PIR_Active) SmsToSend += "PIR Enable.";
  else SmsToSend += "PIR Disable.";
  
  SendTextMessage();
}
//***************************************************************************************************************************//
