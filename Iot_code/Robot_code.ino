#include <SoftwareSerial.h>
SoftwareSerial BT(0,1);
int left1 = 3;
int left2 = 4;
int right1 = 7;
int right2 = 8;
int claw1= 11;
int claw2 = 12;
int hand1= 13;
int hand2= 2;
int ENA = 5;
int ENB = 6;
int ENA1 = 9;
int ENB1 = 10;

int speed = 255;

void setup() {
  BT.begin(9600);
  Serial.begin(9600);

  pinMode(ENA , OUTPUT);
  pinMode(ENB , OUTPUT);
  pinMode(ENA1 , OUTPUT);
  pinMode(ENB1 , OUTPUT);
  pinMode(left1, OUTPUT);
  pinMode(left2, OUTPUT);
  pinMode(right1, OUTPUT);
  pinMode(right2, OUTPUT);
  pinMode(claw1, OUTPUT);
  pinMode(claw2, OUTPUT);
  pinMode(hand1, OUTPUT);
  pinMode(hand2, OUTPUT);
  Serial.println("BLUETOOTH IS READY TO PAIR");
  
}

void loop() {
  if (BT.available()){
    char command = BT.read();
    Serial.println(command);

    if (command == 'w') moveforward();
    else if (command == 's') movebackward();
    else if (command == 'a') turnleft();
    else if (command == 'd') turnright();
    else if (command == 'e') clawopen();
    else if (command == 'r') clawclose();
    else if (command == 'o') handup();
    else if (command == 'p') handdown();
    else stopMotors();
   }

}

void moveforward(){
  analogWrite(ENA , speed);
  analogWrite(ENB , speed);
  digitalWrite(left1 , HIGH);
  digitalWrite(left2 , LOW);
  digitalWrite(right1 , LOW);
  digitalWrite(right2 , HIGH);
}
void movebackward(){
  analogWrite(ENA , speed);
  analogWrite(ENB , speed);
  digitalWrite(left1 , LOW);
  digitalWrite(left2 , HIGH);
  digitalWrite(right1 , HIGH);
  digitalWrite(right2 , LOW);
}
void turnleft(){
  analogWrite(ENA , speed);
  analogWrite(ENB , speed);
  digitalWrite(left1 , LOW);
  digitalWrite(left2 , LOW);
  digitalWrite(right1 , LOW);
  digitalWrite(right2 , HIGH);
}
void turnright(){
  analogWrite(ENA , speed);
  analogWrite(ENB , speed);
  digitalWrite(left1 , HIGH);
  digitalWrite(left2 , LOW);
  digitalWrite(right1 , LOW);
  digitalWrite(right2 , LOW);
}
void clawopen(){
  analogWrite(ENA1 , speed);
  analogWrite(ENB1 , speed);
  digitalWrite(claw1 , HIGH);
  digitalWrite(claw2 , LOW);
  digitalWrite(hand1 , LOW);
  digitalWrite(hand2 , LOW);
}
void clawclose(){
  analogWrite(ENA1 , speed);
  analogWrite(ENB1 , speed);
  digitalWrite(claw1 , LOW);
  digitalWrite(claw2 , HIGH);
  digitalWrite(hand1 , LOW);
  digitalWrite(hand2 , LOW);
}
void handup(){
  analogWrite(ENA1 , speed);
  analogWrite(ENB1 , speed);
  digitalWrite(claw1 , LOW);
  digitalWrite(claw2 , LOW);
  digitalWrite(hand1 , HIGH);
  digitalWrite(hand2 , LOW);
}
void handdown(){
  analogWrite(ENA1 , speed);
  analogWrite(ENB1 , speed);
  digitalWrite(claw1 , LOW);
  digitalWrite(claw2 , LOW);
  digitalWrite(hand1 , LOW);
  digitalWrite(hand2 , HIGH);
}
void stopMotors(){
  digitalWrite(left1,LOW);
  digitalWrite(left2,LOW);
  digitalWrite(right1,LOW);
  digitalWrite(right2,LOW);
  digitalWrite(claw1,LOW);
  digitalWrite(claw2,LOW);
  digitalWrite(hand1,LOW);
  digitalWrite(hand2,LOW);
  analogWrite(ENA , 0);
  analogWrite(ENB , 0);
  analogWrite(ENA1 , 0);
  analogWrite(ENB1 , 0);
}
