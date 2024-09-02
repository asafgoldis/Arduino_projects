#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(3,2);

unsigned char mem[64];
String memm;
String Phone;
String Date;
String Time;
String Msg;
int memcunt = 0;

void setup() {
    gprsSerial.begin(9600); // GPRS shield baud rate
    Serial.begin(9600);
    pinMode(6, OUTPUT);
    delay(500);
}

void loop() {
    /*if (gprsSerial.available()) { // if there is incoming serial data
      while(gprsSerial.available()) {
        mem = gprsSerial.read();
        //Serial.println(memcunt);
        //mem[memcunt++]=gprsSerial.read();     // writing data into array
        //if(memcunt == 64)break;
      }
      //Serial.write(mem, memcunt);            // if no data transmission ends, write buffer to hardware serial port
      Serial.println(mem);
      /*
      switch(Serial.read()) { // read the character
        case 't': // if the character is 't'
          SendTextMessage(); // send the text message
        break;
        case 'd': // if the character is 'd'
          //DialVoiceCall(); // dial a number
          led();
        break;
      }
      */
      //clearBufferArray();              // call clearBufferArray function to clear the storaged data from the array
      //memcunt = 0;                       // set counter of while loop to zero

    //}
    if (gprsSerial.available()) { // if the shield has something to say
      memm = "";
      Phone = "";
      Date = "";
      Time = "";
      Msg = "";
      memcunt = 0;
      while(gprsSerial.available()) {
        mem[memcunt] = gprsSerial.read();
       // memm += String(mem[memcunt]);
        memcunt++;
        delay(10);
      }
      
      //Serial.write(mem, memcunt); // display the output of the shield
      
      for (int i = 0; i<memcunt; i++){
        if (i>8 && i<=21)
          Phone += char(mem[i]);
        else if (i>27 && i<=35)
          Date += char(mem[i]);
        else if (i>36 && i<=44)
          Time += char(mem[i]);
        else if (i>=51)
          Msg += char(mem[i]);
         
          
        memm += char(mem[i]);
      }
      Serial.println(memm);
      Serial.println("Phone = "+ Phone);
      Serial.println("Date = "+ Date);
      Serial.println("Time = "+ Time);
      Serial.println("Msg = "+ Msg);
      
      //Serial.write(gprsSerial.read()); // display the output of the shield
      //memcunt = 0;
   //   Serial.print("memm = ");
   // Serial.println(memm);
    }

/*
    for (int i = 0; i = 64; i++) {
      memm += mem[i];
    }
    Serial.print("memm = ");
    Serial.println(memm);
    */
    /*
    if (digitalRead(6) == HIGH)
      digitalWrite(6, LOW);
    else
      digitalWrite(6, HIGH);
      */
}

  /*
  * Name: SendTextMessage
  * Description: Send a text message to a number
  */
  /*
void SendTextMessage() {
  Serial.println("Sending Text...");
  gprsSerial.print("AT+CMGF=1\r"); // Set the shield to SMS mode
  delay(100);
  // send sms message, the phone number needs to include the country code e.g. if a U.S. phone number such as (540) 898-5543 then the string must be:
  // +15408985543
  gprsSerial.println("AT+CMGS = \"972505577708\"");
  delay(100);
  gprsSerial.println("How are you today?"); //the content of the message
  delay(100);
  gprsSerial.print((char)26);//the ASCII code of the ctrl+z is 26 (required according to the datasheet)
  delay(100);
  gprsSerial.println();
  Serial.println("Text Sent.");
}
*/
  /*
  * Name: DialVoiceCall()
  * Description: Can call/dial a phone number
  */
  /*
void led() {
  if (digitalRead(6) == HIGH)
    digitalWrite(6, LOW);
  else
    digitalWrite(6, HIGH);
}
*/
/*
void DialVoiceCall() {
  gprsSerial.println("ATD+xxxxxxxxxx;");//dial the number, must include country code
  delay(100);
  gprsSerial.println();
}
*/
/*
void clearBufferArray() {              // function to clear buffer array
  for (int i=0; i<memcunt;i++) {
    mem[i]=NULL;                 // clear all index of array with command NULL
  }
}
*/
