#include <Servo.h>

// Motor Parametreleri
#define MOTORR1 2
#define MOTORR2 4
#define MOTORRE 5
#define MOTORL1 7
#define MOTORL2 8
#define MOTORLE 6
unsigned char MotorSpeed = 150;

//Servo direksiyon parametreleri
#define SERVOPIN 3
Servo direksiyonServo;

//Çeşitli parametreler
char receivedChar;
#define BUZZER 9
#define STOPLIGHT A0
#define SIGNALRIGHT A1
#define SIGNALLEFT A2
#define HEADLIGHT A3
#define TRIGGER 12
#define ECHO 11

//-----------Zamanlayıcı Parametreleri-------------
unsigned long currentMillis = 0;

//Duruş lambaları
unsigned long stopLightPreviousMillis = 0;
const unsigned long STOP_LIGHT_DURATION = 3000;

//Sinyal ve dörtlüler
bool signalState = false;
bool fourWaySignalActive = false;
unsigned long signalLightPreviousMillis = 0;
const unsigned long SIGNAL_LIGHT_INTERVAL = 1000;

//Geri uyarı sesi
bool isBackwards = false;
unsigned long buzzerPreviousMillis = 0; 
const unsigned long BACKWARDS_SOUND_INTERVAL = 1000;

//Korna parametreleri
bool hornActive = false; 
unsigned long hornStartMillis = 0; 
const unsigned long HORN_DURATION = 1000;

/*
TODO: 
1 ✔ Durunca kırmızı 3sn yanacak 
2 ✔ Dönüşlerde oto sinyal
3 ✔ Tuş atamasıyla 4lüler
4 ✔ geri giderken BUZZER belirli arallıklarla çalar
5 ✔ BUZZER korna olarak da kullanılabilir
6 geri giderken çok yaklaşırsak BUZZER kullanarak uyarı verir
7 ✔ ileri sol, ileri sağ, geri sol, geri sağ, aynı anda gidebilme
*/

void setup() 
{
  pinMode(MOTORL1, OUTPUT);
  pinMode(MOTORL2, OUTPUT);
  pinMode(MOTORLE, OUTPUT);
  pinMode(MOTORR1, OUTPUT);
  pinMode(MOTORR2, OUTPUT);
  pinMode(MOTORRE, OUTPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(STOPLIGHT, OUTPUT);
  pinMode(SIGNALRIGHT, OUTPUT);
  pinMode(SIGNALLEFT, OUTPUT);
  pinMode(HEADLIGHT, OUTPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  direksiyonServo.attach(SERVOPIN);
  direksiyonServo.write(90);

  Serial.begin(9600);
}

void loop() 
{
  currentMillis = millis();

  // Eğer stop ışığı yanıyorsa ve saniye geçmişse söndür
  if (digitalRead(STOPLIGHT) == HIGH && currentMillis - stopLightPreviousMillis >= STOP_LIGHT_DURATION) 
  {
    digitalWrite(STOPLIGHT, LOW); 
  }
  //Korna aktifse ve zamanı dolduysa kapanır
  if (hornActive && currentMillis - hornStartMillis >= HORN_DURATION) 
  {
    noTone(BUZZER); // BUZZER'ı durdur
    hornActive = false; // Korna durumu kapalı
  }

  handleTurnSignals(); // Direksiyon pozisyonuna göre sinyal ışıklarını yönlendir

  // ------ Bluethood komutları-------
  if (Serial.available() > 0) 
  {
    receivedChar = Serial.read();
  }
  switch(receivedChar)
  {
    case 'F': forward();        break;
    case 'E': forwardRight();   break;
    case 'Q': forwardLeft();    break;
    case 'R': right();          break;
    case 'L': left();           break;
    case 'B': backwards();      break;
    case 'C': backwardsRight(); break;    
    case 'Z': backwardsLeft();  break;
    case 'S': stop();           break;
    case 'M': digitalWrite(HEADLIGHT, HIGH); break;
    case 'm': digitalWrite(HEADLIGHT, LOW);  break;
    case 'N': activateFourWaySignal(true);   break;
    case 'n': activateFourWaySignal(false);  break;
    case 'X': horn(); break;
    case 'Y':         break;
    default:          break;
  } 
  //--------------------------------------

  // Eğer geri gidiyorsak, BUZZER'ı belirli aralıklarla çalıştır/durdur
  if (isBackwards) 
  {
    handleBuzzer();
  }
  // Dörtlü sinyaller aktifse sinyal ışıklarını yönet
  if (fourWaySignalActive) 
  {
    handleFourWaySignals();
  }
}

void handleBuzzer()
{
  if (currentMillis - buzzerPreviousMillis >= BACKWARDS_SOUND_INTERVAL) 
  {
    buzzerPreviousMillis = currentMillis;

    if (digitalRead(BUZZER) == LOW) 
    {
      tone(BUZZER, 1000);
    } 
    else 
    {
      noTone(BUZZER);
    }
  }
}

void handleTurnSignals() 
{
  if (fourWaySignalActive) 
  {
    return; // Eğer dörtlü sinyal aktifse diğer sinyaller çalışmasın
  }

  if (direksiyonServo.read() < 85) // Eğer direksiyon 85 derecenin altında ise sol sinyal
  {
    if (currentMillis - signalLightPreviousMillis >= SIGNAL_LIGHT_INTERVAL) 
    {
      signalLightPreviousMillis = currentMillis;
      signalState = !signalState; // Sinyal ışığının durumunu değiştir
      digitalWrite(SIGNALLEFT, signalState);
    }
  } 
  else if (direksiyonServo.read() > 95) // Eğer direksiyon 95 derecenin üstünde ise sağ sinyal
  {
    if (currentMillis - signalLightPreviousMillis >= SIGNAL_LIGHT_INTERVAL) 
    {
      signalLightPreviousMillis = currentMillis;
      signalState = !signalState; // Sinyal ışığının durumunu değiştir
      digitalWrite(SIGNALRIGHT, signalState);
    }
  } 
  else 
  {
    digitalWrite(SIGNALLEFT, LOW);
    digitalWrite(SIGNALRIGHT, LOW);
  }
}

void handleFourWaySignals()
{
  if (currentMillis - signalLightPreviousMillis >= SIGNAL_LIGHT_INTERVAL) 
  {
    signalLightPreviousMillis = currentMillis;
    signalState = !signalState; // Sinyal ışığının durumunu değiştir
    digitalWrite(SIGNALLEFT, signalState);
    digitalWrite(SIGNALRIGHT, signalState);
  }
}

void activateFourWaySignal(bool activate)
{
  fourWaySignalActive = activate;
  
  if (!activate) 
  {
    // Dörtlü sinyal kapatılırsa her iki sinyal ışığını da kapat
    digitalWrite(SIGNALLEFT, LOW);
    digitalWrite(SIGNALRIGHT, LOW);
  }
}

void horn()
{
  tone(BUZZER, 5000); 
  hornStartMillis = millis();
  hornActive = true;
}

#pragma region Hareket fonksiyonları
void forward() 
{
  isBackwards = false;
  digitalWrite(MOTORR1, HIGH);       // Sağ motorun ileri hareketi aktif
  digitalWrite(MOTORR2, LOW);        // Sağ motorun geri hareketi pasif
  analogWrite(MOTORRE, MotorSpeed);  // Sağ motorun hızı
  digitalWrite(MOTORL1, HIGH);       // Sol motorun ileri hareketi aktif
  digitalWrite(MOTORL2, LOW);        // Sol motorun geri hareketi pasif
  analogWrite(MOTORLE, MotorSpeed);  // Sol motorun hızı
}
void right() 
{
  direksiyonServo.write(70);  //Servonun dönüş hızını belirler
}
void left() 
{
  direksiyonServo.write(110);
}
void forwardRight()
{
  forward();
  right();
}
void forwardLeft()
{
  forward();
  left();
}
void backwards() 
{
  digitalWrite(MOTORR1, LOW);
  digitalWrite(MOTORR2, HIGH);
  analogWrite(MOTORRE, MotorSpeed);
  digitalWrite(MOTORL1, LOW);
  digitalWrite(MOTORL2, HIGH);
  analogWrite(MOTORLE, MotorSpeed);
  isBackwards = true;
}
void backwardsRight()
{
  backwards();
  right();
}
void backwardsLeft()
{
  backwards();
  left();
}
void stop() 
{
  direksiyonServo.write(90); //360 servo ise aktif et yoksa sonsuza kadar döner
  digitalWrite(MOTORR1, LOW);
  digitalWrite(MOTORR2, LOW);
  analogWrite(MOTORRE, 0);
  digitalWrite(MOTORL1, LOW);
  digitalWrite(MOTORL2, LOW);
  analogWrite(MOTORLE, 0);
  // Stop ışığını yak ve zamanlayıcıyı başlat
  digitalWrite(STOPLIGHT, HIGH);
  stopLightPreviousMillis = millis();
  isBackwards = false;
}
#pragma endregion