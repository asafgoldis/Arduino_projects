/* This code works with SIM800L Evb version, DHT11 and MLX90614
 * It sets the module on receiving SMS mode, wait if the user has sent a SMS containing keywords "DHT" or "MLX"
 * Check which one is received then proceed to get data from the chosen sensor and send it via SMS to the programmed phone number
 * And come back to receiving mode.
 * Refer to www.SurtrTech.com for more detaims
 */

#include <SoftwareSerial.h>        //Libraries required for Serial communication, i²c communication, DHT11 and MLX90614
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "DHT.h"
 
#define DHTPIN 7                 //DHT OneWire pin and its type
#define DHTTYPE DHT11

char Received_SMS;              //Here we store the full received SMS (with phone sending number and date/time) as char
short DHT_OK=-1,MLX_OK=-1;      //Used later it shows if there's the word "DHT"/"MLX" within the received SMS "-1" means they are not found

String Data_SMS;               //Here's the SMS that we gonna send to the phone number, it may contain DHT data or MLX data


SoftwareSerial sim800l(2, 3);                     // RX,TX for Arduino and for the module it's TXD RXD, they should be inverted
Adafruit_MLX90614 mlx = Adafruit_MLX90614();      //Declaring MLX instance and DHT
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
   
  sim800l.begin(9600);   //Begin all the communications needed Arduino with PC serial and Arduino with all devices (SIM800L+DHT+MLX)
  Serial.begin(9600);     
  mlx.begin(); 
  dht.begin();
  Serial.println("Starting ...");
  delay(3000);         //Delay to let the module connect to network, can be removed
  ReceiveMode();       //Calling the function that puts the SIM800L moduleon receiving SMS mode
  
}



void loop() {
  
  String RSMS;             //We add this new variable String type, and we put it in loop so everytime gets initialized
                           //This is where we put the Received SMS, yes above there's Recevied_SMS variable, we use a trick below
                           //To concatenate the "char Recevied_SMS" to "String RSMS" which makes the "RSMS" contains the SMS received but as a String
                           //The recevied SMS cannot be stored directly as String
  
    while(sim800l.available()>0){       //When SIM800L sends something to the Arduino... problably the SMS received... if something else it's not a problem
        
        Received_SMS=sim800l.read();  //"char Received_SMS" is now containing the full SMS received
        Serial.print(Received_SMS);   //Show it on the serial monitor (optional)     
        RSMS.concat(Received_SMS);    //concatenate "char received_SMS" to RSMS which is "empty"
        DHT_OK=RSMS.indexOf("DHT");   //And this is why we changed from char to String, it's to be able to use this function "indexOf"
        MLX_OK=RSMS.indexOf("MLX");   //"indexOf function looks for the substring "x" within the String (here RSMS) and gives us its index or position
                                      //For example if found at the beginning it will give "0" after 1 character it will be "1"
                                      //If it's not found it will give "-1", so the variables are integers
        
    }
    
  if(DHT_OK!=-1){                         //If "DHT" word is found within the SMS, it means that DHT_OK have other value than -1 so we can proceed
    Serial.println("found DHT");          //Shows on the serial monitor "found DHT" (optional)
    float h = dht.readHumidity();         //Read temperature and humidity
    float t = dht.readTemperature();
    Serial.print("DHT11 Temperature = "); Serial.print(t); Serial.print("*C      DHT11 Humidity = "); Serial.print(h); Serial.println(" %");
                                          //Show it on the serial monitor also optional

    Data_SMS = "DHT11\nTemp = "+String(t,1)+" C"+" \nHumidity ="+String(h,1)+" %";       //Prepare the SMS to send, it contains some strings like "DHT" "Temperature"...
                                                                                         //And then the values read
    
    Send_Data();                      //This function set the sending SMS mode, prepare the phone number to which we gonna send, and send "Data_SMS" String
    ReceiveMode();                   //Come back to Receving SMS mode and wait for other SMS
    
    DHT_OK=-1;                      //If the DHT is found the variable should be reset to -1 otherwise it will be kept as !=-1 and will send SMS over and over
    MLX_OK=-1;                      //Maybe not required... I did a lot of tests and maybe at the beginning the RSMS string kept concating and MLX word was kept there
                                    //And at this point I'm too lazy to reupload the code without it and test...
  }


  if(MLX_OK!=-1){                   //Same thing if the "MLX" word is found, Serial.print things are optionnal
  delay(1000);
  Serial.println("found MLX");
  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");

  Data_SMS = "MLX\nAmbient = "+String(mlx.readAmbientTempC(),1)+" C"+" \nObject ="+String(mlx.readObjectTempC(),1)+" C";

  Send_Data();
  ReceiveMode();
  
  MLX_OK=-1;
  DHT_OK=-1;
}
     

}


void Serialcom() //This is used with ReceiveMode function, it's okay to use for tests with Serial monitor
{
  delay(500);
  while(Serial.available())                                                                      
  {
    sim800l.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(sim800l.available())                                                                      
  {
    Serial.write(sim800l.read());//Forward what Software Serial received to Serial Port
  }
}

void ReceiveMode(){       //Set the SIM800L Receive mode
  
  sim800l.println("AT"); //If everything is Okay it will show "OK" on the serial monitor
  Serialcom();
  sim800l.println("AT+CMGF=1"); // Configuring TEXT mode
  Serialcom();
  sim800l.println("AT+CNMI=2,2,0,0,0"); //Configure the SIM800L on how to manage the Received SMS... Check the SIM800L AT commands manual
  Serialcom();
}

void Send_Data()
{
  Serial.println("Sending Data...");     //Displays on the serial monitor...Optional
  sim800l.print("AT+CMGF=1\r");          //Set the module to SMS mode
  delay(100);
  sim800l.print("AT+CMGS=\"+***********\"\r");  //Your phone number don't forget to include your country code example +212xxxxxxxxx"
  delay(500);  
  sim800l.print(Data_SMS);  //This string is sent as SMS
  delay(500);
  sim800l.print((char)26);//Required to tell the module that it can send the SMS
  delay(500);
  sim800l.println();
  Serial.println("Data Sent.");
  delay(500);

}
