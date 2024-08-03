#include <Servo.h>

// Motor Parametreleri
#define MotorR1 7
#define MotorR2 6
#define MotorRE 9
#define MotorL1 5
#define MotorL2 4
#define MotorLE 3
unsigned char MotorSpeed = 150;
//Servo direksiyon parametreleri
#define ServoPin 10
Servo direksiyonServo;
int servoPos = 90;
//Bluethood parametreleri
char receivedChar;
//Çeşitli parametreler
#define Far 13
#define Buzzer 11


void setup() {
  pinMode(MotorL1, OUTPUT);
  pinMode(MotorL2, OUTPUT);
  pinMode(MotorLE, OUTPUT);
  pinMode(MotorR1, OUTPUT);
  pinMode(MotorR2, OUTPUT);
  pinMode(MotorRE, OUTPUT);

  pinMode(Far, OUTPUT);
  pinMode(Buzzer, OUTPUT);

  direksiyonServo.attach(ServoPin);
  direksiyonServo.write(servoPos);

  Serial.begin(9600);
}
void loop() 
{
  if (Serial.available() > 0) 
  {
    receivedChar = Serial.read();
  }
  switch(receivedChar)
  {
    case 'F': ileri(); break;
    case 'B': geri();  break;      
    case 'R': sag();   break;
    case 'L': sol();   break;
    case 'M': digitalWrite(Far, HIGH); break;
    case 'm': digitalWrite(Far, LOW);  break;
    case 'S': dur();   break;
    case 'X': tone(Buzzer, 1000); break;
    case 'Y': noTone(Buzzer); break;
  } 
}

#pragma region Hareket fonksiyonları
void ileri() 
{
  digitalWrite(MotorR1, HIGH);       // Sağ motorun ileri hareketi aktif
  digitalWrite(MotorR2, LOW);        // Sağ motorun geri hareketi pasif
  analogWrite(MotorRE, MotorSpeed);  // Sağ motorun hızı
  digitalWrite(MotorL1, HIGH);       // Sol motorun ileri hareketi aktif
  digitalWrite(MotorL2, LOW);        // Sol motorun geri hareketi pasif
  analogWrite(MotorLE, MotorSpeed);  // Sol motorun hızı
}
void sag() 
{
  direksiyonServo.write(70);
}
void sol() 
{
  direksiyonServo.write(110);
}
void geri() {
  digitalWrite(MotorR1, LOW);
  digitalWrite(MotorR2, HIGH);
  analogWrite(MotorRE, MotorSpeed);
  digitalWrite(MotorL1, LOW);
  digitalWrite(MotorL2, HIGH);
  analogWrite(MotorLE, MotorSpeed);
}
void dur() {
  direksiyonServo.write(90); //360 servo ise aktif et yoksa sonsuza kadar döner
  digitalWrite(MotorR1, LOW);
  digitalWrite(MotorR2, LOW);
  analogWrite(MotorRE, 0);
  digitalWrite(MotorL1, LOW);
  digitalWrite(MotorL2, LOW);
  analogWrite(MotorLE, 0);
}
#pragma endregion