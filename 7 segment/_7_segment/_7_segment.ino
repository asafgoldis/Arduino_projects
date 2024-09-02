int const data =2;
int const clk =3;
int const enable =4;

int num_0 []={1,1,0,0,1,1,1,1};
//int num_1=00100001;
//int num_2=11001011;
//int num_3=01101011;
int num_4 []={0,0,1,0,1,1,0,1};
//int num_5=01101110;
//int num_6=11101110;
//int num_7=00100011;
//int num_8=11101111;
//int num_9=01101111;

void setup() {
  pinMode(data, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(clk, OUTPUT);
}

//int arr[]= {num_0,num_1,num_2,num_3,num_4,num_5,num_6,num_7,num_8,num_9};

//int arr[]= {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7c,0x07,0x7f,0x67};

void loop() {
  for(int num=0; num<10; num++) {
    for(int i=0;i<8;i++) {
      digitalWrite(clk, LOW);
      digitalWrite(data, num_0 [i]);
      digitalWrite(clk, HIGH);
    }
    digitalWrite(enable, HIGH);
    delay(500);
  }
}
