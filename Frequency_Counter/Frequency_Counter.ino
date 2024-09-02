#include <LiquidCrystal.h>
 
LiquidCrystal lcd(13,12,11,10,9,8);
long freq, tempo;
int pulsos;
boolean pulso;

void setup() {
  pulso=HIGH;
  pinMode(2,INPUT);
  lcd.begin(20, 4);
  lcd.setCursor(0,0);
  lcd.print("Frequency =");
}
 
void loop() {
  tempo = millis();
  if(digitalRead(2)==HIGH){
    if(pulso==HIGH){
      pulsos = pulsos + 1;
    }
    pulso=LOW; 
  }
  else{
    pulso=HIGH;
  }
 
  if(tempo%2000==0){
    freq = pulsos/2;
    lcd.setCursor(12,0);
    lcd.print(freq);
    lcd.print("Hz");
    pulsos=0;  
  }
}
