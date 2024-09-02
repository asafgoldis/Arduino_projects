
int up = 0;
int next = 0;
int dey = 1;
int month = 1;
int year = 2010;

void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  
  Serial.print("set");
  Serial.print("  ");
  Serial.print("dey");
  Serial.print("/");
  Serial.print("month");
  Serial.print("/");
  Serial.println("year");
    
}

void loop() {
  while (digitalRead(8)==0 && digitalRead(9)==0);
  if (digitalRead(8)==1 && next==0){
    dey++;
    if (dey>31){
      dey=1;
    }
  }
  else if (digitalRead(8)==1 && next==1){
    month++;
    if (month>12){
      month=1;
    }
  }  
  else if (digitalRead(8)==1 && next==2){
    year++;
    if (year>2050){
        year=2015;
    }
  }
  else if (digitalRead(9)==1){
    next++;
    if (next>2){
      next=0;
    }
  }
  
if (next==0)Serial.print("day:  ");
else if (next==1) Serial.print("month:");
else Serial.print("year: ");
  Serial.print("    ");
  Serial.print(dey);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.println(year);

  while (digitalRead(8)==1 || digitalRead(9)==1); // כל עוד הלחצן במצב 'לחוץ' המתן
}
