int sensorPin = A0;
int lightPin = A1;
int ledPin = 13; 
int ido = 0;
int light=0;

void setup() {
  Serial.begin(9600);
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // read the value from the sensor:
  ido = analogRead(sensorPin);
  light = analogRead(lightPin);
  // turn the ledPin on
  //digitalWrite(ledPin, HIGH);
  // stop the program for <sensorValue> milliseconds:
  //delay(ido);
  // turn the ledPin off:
  //digitalWrite(ledPin, LOW);
  // stop the program for for <sensorValue> milliseconds:
  //delay(ido);
  
  if(ido<1000){
    digitalWrite(ledPin, HIGH);
    delay(light*10);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
  Serial.print("light is: "); Serial.print(light*10); Serial.print(", atai is: "); Serial.println(ido);
  delay(50);
}
