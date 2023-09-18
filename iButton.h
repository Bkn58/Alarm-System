#include <OneWire.h>
#include "pitches.h"

//#define iButtonPin A5      // Линия data ibutton
#define iButtonPin A7      // Линия data ibutton
#define ACpin 6            // Вход аналогового компаратора 3В для Cyfral/Metacom
#define speakerPin 9       // Спикер, он же buzzer, он же beeper

OneWire ibutton (iButtonPin); 
byte keyID[8];                             // временный буфер
bool preBtnPinSt = HIGH;
enum emkeyType {keyUnknown, keyDallas, keyTM2004, keyCyfral, keyMetacom};    // тип оригинального ключа  
emkeyType keyType;
//***************** зуки****************
void Sd_ReadOK() {  // звук ОК
  for (int i=400; i<6000; i=i*1.5) { tone(speakerPin, i); delay(20); }
  noTone(speakerPin);
}

void Sd_StartOK(){   // звук "Успешное включение"
  tone(speakerPin, NOTE_A7,); delay(100);
  tone(speakerPin, NOTE_G7); delay(100);
  tone(speakerPin, NOTE_E7); delay(100); 
  tone(speakerPin, NOTE_C7); delay(100);  
  tone(speakerPin, NOTE_D7); delay(100); 
  tone(speakerPin, NOTE_B7); delay(100); 
  tone(speakerPin, NOTE_F7); delay(100); 
  tone(speakerPin, NOTE_C7); delay(100);
  noTone(speakerPin); 
}
void Sd_Button(){   // звук "распознан iButton"
  tone(speakerPin, NOTE_C7); delay(100);
  tone(speakerPin, NOTE_F7); delay(100); 
  tone(speakerPin, NOTE_B7); delay(100); 
  tone(speakerPin, NOTE_D7); delay(100); 
  tone(speakerPin, NOTE_C7); delay(100);  
  tone(speakerPin, NOTE_E7); delay(100); 
  tone(speakerPin, NOTE_G7); delay(100);
  tone(speakerPin, NOTE_A7); delay(100);
  noTone(speakerPin); 
}
/*
void Sd_WriteStep(){  // звук "очередной шаг"
  for (int i=2500; i<6000; i=i*1.5) { tone(speakerPin, i); delay(10); }
  noTone(speakerPin);
}

void Sd_ErrorBeep() {  // звук "ERROR"
  for (int j=0; j <3; j++){
    for (int i=1000; i<2000; i=i*1.1) { tone(speakerPin, i); delay(10); }
    delay(50);
    for (int i=1000; i>500; i=i*1.9) { tone(speakerPin, i); delay(10); }
    delay(50);
  }
  noTone(speakerPin);
}
*/

//************ Cyfral ***********************
unsigned long pulseAComp(bool pulse, unsigned long timeOut = 20000){  // pulse HIGH or LOW
  bool AcompState;
  unsigned long tStart = micros();
  do {
    AcompState = ACSR & _BV(ACO);  // читаем флаг компаратора
    if (AcompState == pulse) {
      tStart = micros();
      do {
        AcompState = ACSR & _BV(ACO);  // читаем флаг компаратора
        if (AcompState != pulse) return (long)(micros() - tStart);  
      } while ((unsigned long)(micros() - tStart) < timeOut);
      return 0;                                                 //таймаут, импульс не вернуся оратно
    }             // end if
  } while ((unsigned long)(micros() - tStart) < timeOut);
  return 0;
}

void ACsetOn(){
  ADCSRA &= ~(1<<ADEN);      // выключаем ADC
  ADCSRB |= (1<<ACME);        //включаем AC
  //ADMUX = 0b00000101;        // подключаем к AC Линию A5
  ADMUX = 0b00000111;        // подключаем к AC Линию 0b00000111 A7
}

bool read_cyfral(byte* buf, byte CyfralPin){
  unsigned long ti; byte j = 0;
  digitalWrite(CyfralPin, LOW); pinMode(CyfralPin, OUTPUT);  //отклчаем питание от ключа
  delay(200);
  ACsetOn();    //analogComparator.setOn(0, CyfralPin); 
  pinMode(CyfralPin, INPUT_PULLUP);  // включаем пиание Cyfral
  for (byte i = 0; i<36; i++){    // чиаем 36 bit
    ti = pulseAComp(HIGH);
    if ((ti == 0) || (ti > 200)) break;                      // not Cyfral
    //if ((ti > 20)&&(ti < 50)) bitClear(buf[i >> 3], 7-j);
    if ((ti > 50) && (ti < 200)) bitSet(buf[i >> 3], 7-j);
    j++; if (j>7) j=0; 
  }
  if (ti == 0) return false;
  if ((buf[0] >> 4) != 0b1110) return false;   /// not Cyfral
  byte test;
  for (byte i = 1; i<4; i++){
    test = buf[i] >> 4;
    if ((test != 1)&&(test != 2)&&(test != 4)&&(test != 8)) return false;
    test = buf[i] & 0x0F;
    if ((test != 1)&&(test != 2)&&(test != 4)&&(test != 8)) return false;
  }
  return true;
}

bool searchCyfral(){
  for (byte i = 0; i < 8; i++) keyID[i] = 0;
  bool rez = read_cyfral(keyID, iButtonPin);
  if (!rez) return false; 
  keyType = keyCyfral;
  for (byte i = 0; i < 8; i++) {
    Serial.print(keyID[i], HEX); Serial.print(":");
    //keyID[i] = addr[i];                               // копируем прочтенный код в ReadID
  }
  Serial.println(" Type: Cyfral ");
  return true;  
}
boolean cyfral() {
  if (searchCyfral()) {  // запускаем поиск cyfral
	//digitalWrite(B_Led, HIGH); // внутренний LED (13 pin) и внешний на контакторе
    Sd_ReadOK();
	//digitalWrite(B_Led, LOW); 
	return true;
  }
//  delay(300);
  return false;
}//*************************************




