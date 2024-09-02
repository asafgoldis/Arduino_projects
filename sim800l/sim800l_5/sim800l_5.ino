#include <SoftwareSerial.h>
#include <String.h>

SoftwareSerial gprsSerial(3,2); // RX, TX

#define PHONE_NUM +972505577708
#define RELAY_PIN_1 8
#define RELAY_PIN_2 7
#define RELAY_PIN_3 6
#define RELAY_PIN_4 5
#define PIR_PIN 4
#define LED_PIN 13
#define BUTTON_PIN 9

int mm=0;
short cmd0=-1,cmd1=-1,cmd2=-1,cmd3=-1,cmd4=-1,cmd5=-1,cmd6=-1,cmd7=-1,cmd8=-1,cmd9=-1;
char Received_SMS;              //Here we store the full received SMS (with phone sending number and date/time) as char

String Sms;
String Phone;
String Date;
String Time;
int buttonState = 0;
int Cmd_num = -1;
String Commands[10] = {"relay1on", "relay2on", "relay3on", "relay4on",
                       "relay1off", "relay2off", "relay3off", "relay4off", 
                       "ledOn",
                       "ledOff"};

void setup() {
    gprsSerial.begin(9600); // GPRS shield baud rate
    Serial.begin(9600);

  //  gprsSerial.println("AT"); //Once the handshake test is successful, it will back to OK
  //  gprsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  //  gprsSerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    ReceiveMode();       //Calling the function that puts the SIM800L moduleon receiving SMS mode
    pinMode(RELAY_PIN_1, OUTPUT); digitalWrite(RELAY_PIN_1, HIGH);
    pinMode(RELAY_PIN_2, OUTPUT); digitalWrite(RELAY_PIN_2, HIGH);
    pinMode(RELAY_PIN_3, OUTPUT); digitalWrite(RELAY_PIN_3, HIGH);
    pinMode(RELAY_PIN_4, OUTPUT); digitalWrite(RELAY_PIN_4, HIGH);
    pinMode(LED_PIN, OUTPUT); digitalWrite(LED_PIN, LOW);
    
    pinMode(BUTTON_PIN, INPUT);
    pinMode(PIR_PIN, INPUT);
    Serial.println("Start");
    delay(500);
}

void loop() {
  mm++;
  Serial.println(mm);
  if (gprsSerial.available()) { // if the shield has something to say
    GetMsg();
    //Cmd_num = Compare(Sms);
    //Action(Cmd_num);
    Action();
    ReceiveMode();
  }
  
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == 0) {
    delay(300);
    SendTextMessage(); 
  }
  
  delay(200);
}

void GetMsg() {
  Serial.println("available...");


  String RSMS;             //We add this new variable String type, and we put it in loop so everytime gets initialized
                           //This is where we put the Received SMS, yes above there's Recevied_SMS variable, we use a trick below
                           //To concatenate the "char Recevied_SMS" to "String RSMS" which makes the "RSMS" contains the SMS received but as a String
                           //The recevied SMS cannot be stored directly as String
  
    while(gprsSerial.available()>0){      //When SIM800L sends something to the Arduino... problably the SMS received... if something else it's not a problem
        
        Received_SMS=gprsSerial.read();   //"char Received_SMS" is now containing the full SMS received
        Serial.print(Received_SMS);       //Show it on the serial monitor (optional)     
        RSMS.concat(Received_SMS);        //concatenate "char received_SMS" to RSMS which is "empty"
        cmd0=RSMS.indexOf(Commands[0]);   //And this is why we changed from char to String, it's to be able to use this function "indexOf"
        cmd1=RSMS.indexOf(Commands[1]);   //"indexOf function looks for the substring "x" within the String (here RSMS) and gives us its index or position
        cmd2=RSMS.indexOf(Commands[2]);   //For example if found at the beginning it will give "0" after 1 character it will be "1"
        cmd3=RSMS.indexOf(Commands[3]);   //If it's not found it will give "-1", so the variables are integers
        cmd4=RSMS.indexOf(Commands[4]);
        cmd5=RSMS.indexOf(Commands[5]);
        cmd6=RSMS.indexOf(Commands[6]);
        cmd7=RSMS.indexOf(Commands[7]);
        cmd8=RSMS.indexOf(Commands[8]);
        cmd9=RSMS.indexOf(Commands[9]);
    }


  /*
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
  
  //Serial.println("Phone = "+ Phone);
  //Serial.println("Date = "+ Date);
  //Serial.println("Time = "+ Time);
  //Serial.println("Sms = "+ Sms);
  Serial.println(Sms);
  */
}

void Action() {

  if      (cmd0 != -1) digitalWrite(RELAY_PIN_1, LOW);
  else if (cmd1 != -1) digitalWrite(RELAY_PIN_2, LOW);
  else if (cmd2 != -1) digitalWrite(RELAY_PIN_3, LOW);
  else if (cmd3 != -1) digitalWrite(RELAY_PIN_4, LOW);
  
  else if (cmd4 != -1) digitalWrite(RELAY_PIN_1, HIGH);
  else if (cmd5 != -1) digitalWrite(RELAY_PIN_2, HIGH);
  else if (cmd6 != -1) digitalWrite(RELAY_PIN_3, HIGH);
  else if (cmd7 != -1) digitalWrite(RELAY_PIN_4, HIGH);
  
  else if (cmd8 != -1) digitalWrite(LED_PIN, HIGH);
  else if (cmd9 != -1) digitalWrite(LED_PIN, LOW);
}
/*
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
*/
void SendTextMessage() {
  Serial.println("Sending Text...");
  
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  gprsSerial.print("AT+CMGS=\"972505577708\"\r");  
  delay(500);
  gprsSerial.print("PIR Alart!! \r"); //the content of the message
  delay(500);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(500);
  gprsSerial.println();
  Serial.println("Text Sent.");
  delay(500);
  
  ReceiveMode();                   //Come back to Receving SMS mode and wait for other SMS
}

void Serialcom() { //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
  delay(500);
  while(Serial.available()) {
    gprsSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(gprsSerial.available()) {
    Serial.write(gprsSerial.read());//Forward what Software Serial received to Serial Port
  }
}

void ReceiveMode(){       //Set the SIM800L Receive mode
  gprsSerial.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  gprsSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  Serialcom();
  gprsSerial.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
}


int Compare(String String1) {
  String String2 = "";
  int str1Length = String1.length()-1;
  int str2Length = 0;
  char char1, char2;
  int cmd_num = -1;
  
  for (int cmd=0; cmd<sizeof(Commands); cmd++) {
    Serial.println("cmd=" + cmd);
    String2 = Commands[cmd];
    str2Length = String2.length();
    
    if (str1Length == str2Length) {
      for (int i=0; i<str1Length; i++) {
        char1 = String1.charAt(i);
        char2 = String2.charAt(i);
        if (char1 == char2)
          cmd_num = cmd;
        else {
          cmd_num = -1;
          break;
        }
      }
      if (cmd_num != -1)
        return cmd_num;
    }
  }
  return -1;
}
