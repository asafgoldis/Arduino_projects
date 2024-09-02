//***************************************************************************************************************************//
//                                            Remote Control System V2.0
//***************************************************************************************************************************//
//הגדרה של ריליי 1+2 להזזת מנןע ימינה ושמאלה למשך 5 שניות

//    IO Connaction:
// A0 - NOT CONNECTED
// A1 - NOT CONNECTED
// A2 - NOT CONNECTED
// A3 - Button input
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
// D9 - NOT CONNECTED
// D10 - NOT CONNECTED
// D11 - PIR led (220 Ohm resistor)
// D12 - PIR input
// D13 - Led
//***************************************************************************************************************************//
#include <SoftwareSerial.h>
#include <String.h>
//***************************************************************************************************************************//
// Define IO
#define NUM_OF_CMD 16

#define PHONE_NUM +972505577708
#define BUTTON_PIN A3
#define RELAY_PIN_4 5
#define RELAY_PIN_3 6
#define RELAY_PIN_2 7
#define RELAY_PIN_1 8
#define PIR_LED_PIN 11
#define PIR_PIN 12                    // the pin that the PIR sensor is atteched to
#define LED_PIN 13
//***************************************************************************************************************************//
SoftwareSerial gprsSerial(3,2);       // RX, TX

int CMD[NUM_OF_CMD];
int dir = 0;
String SmsToSend;
String Commands[NUM_OF_CMD] = {"Left", "Right", "Relay3on", "Relay4on", 
                               "Relay1off", "Relay2off", "Relay3off", "Relay4off", 
                               "LedOn", "LedOff", "AllRelayOn", "AllRelayOff", "PirEnable", "PirDisable", "Status", "Cmd"};
char Received_SMS;                    //Here we store the full received SMS (with phone sending number and date/time) as char
bool buttonState = LOW;
bool PIR_state = LOW;                 // by default, no motion detected
bool PIR_val = LOW;                   // variable to store the sensor status (value)
bool PIR_Active = false;

unsigned long loop_timer;

//long unsigned int lowIn;
//long unsigned int pause = 5000;
//lowIn = millis();
//int cnt = 0;
//***************************************************************************************************************************//
void setup() {
  gprsSerial.begin(9600); // GPRS shield baud rate
  Serial.begin(9600);
  
  for (int i=0; i<NUM_OF_CMD; i++) {
    CMD[i] = -1;
  }
  pinMode(RELAY_PIN_1, OUTPUT); digitalWrite(RELAY_PIN_1, HIGH);
  pinMode(RELAY_PIN_2, OUTPUT); digitalWrite(RELAY_PIN_2, HIGH);
  pinMode(RELAY_PIN_3, OUTPUT); digitalWrite(RELAY_PIN_3, HIGH);
  pinMode(RELAY_PIN_4, OUTPUT); digitalWrite(RELAY_PIN_4, HIGH);
  pinMode(LED_PIN    , OUTPUT); digitalWrite(LED_PIN    , LOW);
  pinMode(BUTTON_PIN , INPUT_PULLUP);
  pinMode(PIR_PIN    , INPUT );
  
  ReceiveMode();                  //Calling the function that puts the SIM800L moduleon receiving SMS mode
  //Serial.println("Start");
  loop_timer = millis();
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
  
  if (dir != 0) {
    Direction();
  }
  
  buttonState = digitalRead(BUTTON_PIN);
  if (!buttonState) {
    delay(300);
    SmsToSend = "buttonPress";
    SendTextMessage(); 
  }
  
  if (PIR_Active) ReadPIR();
  else digitalWrite(PIR_LED_PIN, LOW);       // turn LED OFF
    
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
  //if      (CMD[0] != -1) digitalWrite(RELAY_PIN_1, LOW);
  //else if (CMD[1] != -1) digitalWrite(RELAY_PIN_2, LOW);
  if      (CMD[0] != -1) {  // Left
    dir = 1;
    loop_timer = millis();
  }
  else if (CMD[1] != -1) {  // Right
    dir = 2;
    loop_timer = millis();
  }
  else if (CMD[2] != -1) digitalWrite(RELAY_PIN_3, LOW);    // Relay 3 on
  else if (CMD[3] != -1) digitalWrite(RELAY_PIN_4, LOW);    // Relay 4 on
  
  else if (CMD[4] != -1) digitalWrite(RELAY_PIN_1, HIGH);   // Relay 1 off
  else if (CMD[5] != -1) digitalWrite(RELAY_PIN_2, HIGH);   // Relay 2 off
  else if (CMD[6] != -1) digitalWrite(RELAY_PIN_3, HIGH);   // Relay 3 off
  else if (CMD[7] != -1) digitalWrite(RELAY_PIN_4, HIGH);   // Relay 4 off
  
  else if (CMD[8] != -1) digitalWrite(LED_PIN, HIGH);       // Led  On
  else if (CMD[9] != -1) digitalWrite(LED_PIN, LOW);        // Led off

  else if (CMD[10] != -1) {                                 // All Relay On
    //digitalWrite(RELAY_PIN_1, LOW); delay(250);
    //digitalWrite(RELAY_PIN_2, LOW); delay(250);
    digitalWrite(RELAY_PIN_3, LOW); delay(250);
    digitalWrite(RELAY_PIN_4, LOW);
  }
  else if (CMD[11] != -1) {                                 // All Relay Off
    //digitalWrite(RELAY_PIN_1, HIGH); delay(250);
    //digitalWrite(RELAY_PIN_2, HIGH); delay(250);
    digitalWrite(RELAY_PIN_3, HIGH); delay(250);
    digitalWrite(RELAY_PIN_4, HIGH);
  }
  else if (CMD[12] != -1) PIR_Active = true;                // Pir Enable
  else if (CMD[13] != -1) PIR_Active = false;               // Pir Disable
  else if (CMD[14] != -1) Status();                         // Status check
  else if (CMD[15] != -1) {                                 // Cmd List
    // SmsToSend = "\"Relay1on\", \"Relay2on\", \"Relay3on\", \"Relay4on\", \"Relay1off\", \"Relay2off\", \"Relay3off\", \"Relay4off\", \"LedOn\", \"LedOff\", \"AllRelayOn\", \"AllRelayOff\", \"PirEnable\", \"PirDisable\", \"Status\" \"Cmd\"";
    SmsToSend = "\"Left\", \"Right\", \"Relay3on\", \"Relay4on\", \"Relay3off\", \"Relay4off\", \"LedOn\", \"LedOff\", \"AllRelayOn\", \"AllRelayOff\", \"PirEnable\", \"PirDisable\", \"Status\" \"Cmd\"";
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
  if (!digitalRead(RELAY_PIN_1)) SmsToSend += "Left Moving, ";
  else SmsToSend += "No Left Moving, ";
  if (!digitalRead(RELAY_PIN_2)) SmsToSend += "Right Moving, ";
  else SmsToSend += "No Right Moving, ";
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
void Direction() {
  if (loop_timer + 5000 > millis()) {
    if (dir == 1) {
      digitalWrite(RELAY_PIN_1, LOW);
      digitalWrite(RELAY_PIN_2, HIGH);
    }
    else if (dir == 2) {
      digitalWrite(RELAY_PIN_1, HIGH);
      digitalWrite(RELAY_PIN_2, LOW);
    }
  }
  else {
    dir = 0;
    digitalWrite(RELAY_PIN_1, HIGH);
    digitalWrite(RELAY_PIN_2, HIGH);
  }
}
//***************************************************************************************************************************//
