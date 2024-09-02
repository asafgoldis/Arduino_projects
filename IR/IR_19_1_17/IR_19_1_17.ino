
int const IRread = A0;
int IRVal; 

void setup() {
  pinMode(IRread, INPUT);
  Serial.begin(9600); 
}

void loop() {
  IRVal = analogRead(IRread); 
  Serial.print("IRVal: "); 
  Serial.println(IRVal);
  delay(100);

}
