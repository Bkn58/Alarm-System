#include <SoftwareSerial.h>
#include <EEPROM.h>
//#include <Wire.h>
#include <INA219.h>
#include "printfloat.h"
#include <iarduino_DHT.h>   // подключаем библиотеку для работы с датчиком DHT
#include <SPI.h>
#include <MFRC522.h>
#include "pitches.h"

#define DHTPIN 7           // пин подключения контакта DATA
#define DHTTYPE DHT11      // DHT 11
iarduino_DHT dht(DHTPIN);  // объявляем  переменную для работы с датчиком DHT, указывая номер цифрового вывода к которому подключён датчик (сейчас 2pin)
#define B_Led A1           // светодиод
#define Door 5             // герконовый датчик двери
#define speakerPin 9       // buzzer на плате ардуино
#define boozerPin A3       // boozer около двери

#ifndef MAX_LEN_BUFF
#define MAX_LEN_BUFF 256  //размер приемного буфера сообщений от SIM900
#endif

/*
+CMTI: "SM",1 -- приход SMS под номером 1
at+cmgr=1,0   -- запрос текста SMS
              -- ответ на запрос текста SMS
+CMGR: "REC UNREAD","+79131234567","","23/02/21,16:09:33+28"
Test

OK
---------------------------------------------------------
at+cmgd=1,0    -- удаление SMS номер 1

OK
---------------------------------------------------------
atd+ +79136513814;  -- звонок на номер

OK

NO ANSWER  -- ответ, если не взяли трубку
BUSY       -- ответ, если абонент занят (или сбросил звонок)
NO DIALTONE-- нет звонка вызова
NO CARRIER -- нет несущей (неправильно набран номер)
CONNECT    -- абонент поднял трубку
---------------------------------------------------------
AT+CMGF=1  -- установка текстового режима смс-сообщений
AT+CMGS="+79136513814" — отправка смс. После ввода команды 
выдает приглашение ">" после чего можно вводить текст сообщений. 
Завершается символом (0x1a) ESC или Ctrl-Z.
---------------------------------------------------------
AT+CCLK?   -- получить текущее время. Вернет текущее время в виде: +CCLK: «11/04/11,01:46:33+00»
AT+CCLK=«yy/mm/dd,hh:mm:ss+zz» -- установить время. Обязательно в таком формате и с ведущими нулями.
---------------------------------------------------------
at+cstt="GPB","",""
OK

at+ciicr   --- соединение с беспроводной сетью. 
           Проверяемая SIM карта должна иметь предоплаченный объем данных или положительный баланс
OK

at+cifsr   --- получить ip-адрес
172.20.9.200

AT+SAPBR=1,1 -- установка GPRS связи
+CREG: 5

AT+SAPBR=2,1 -- полученный IP-адрес
+SAPBR: 1,1,"172.25.213.236"

at+cipstart="TCP","www.yandex.ru","80"
OK
CONNECT OK

at+cipsend  --- передать запрос на сервер
> test      ---- текст сообщения
http://f0795047.xsph.ru/sim_message.php/?l=f0795047_bkn&w=652532&y=23-04-02+14:00:00&b=99&a=none&r=disarm&d=close&p=ok&c=100&t=23&h=30&u=0&o=0&g=200
0x1a        --- окончание сообщения
SEND OK
------------взято с сайта--https://www.edaboard.com/threads/problem-sending-post-to-webserver-from-sim900-with-at-commands.281314/
ждем сообщения
+CREG: 5
или ответа на AT+CREG?
+CREG: 1,5
OK

at+cstt="GPB","",""  --- setup
AT + SAPBR= 3,1, "CONTYPE", "GPRS"
AT + SAPBR = 1,1 --- может быть ответ "+CME ERROR: operation not allowed"---
AT + HTTPINIT
AT+ HTTPPARA="CID",1
AT + HTTPPARA="URL","http://f0795047.xsph.ru/sim_message.php/?l=f0795047_bkn&w=652532&y=23-04-02+14:00:00&b=99&a=none&r=disarm&d=close&p=ok&c=100&t=23&h=30&u=0&o=0&g=200"
AT + HTTPACTION=0
AT + HTTPTERM
                 -----------------протокол-----------------
20:22:55.140 -> Start
20:23:01.362 -> AT
20:23:01.362 -> 
20:23:01.362 -> OK
20:23:01.362 -> 
20:24:11.310 -> 
20:24:11.310 -> +CREG: 5
20:24:29.390 -> at+cstt="GPB","",""
20:24:29.390 -> 
20:24:29.390 -> OK
20:24:39.162 -> AT + SAPBR= 3,1, "CONTYPE", "GPRS"
20:24:39.162 -> 
20:24:39.162 -> OK
20:24:48.731 -> AT + SAPBR = 1,1
20:24:49.773 -> 
20:24:49.773 -> OK
20:24:57.648 -> AT + HTTPINIT
20:24:57.648 -> 
20:24:57.648 -> OK
20:25:09.982 -> AT+ HTTPPARA="CID",1
20:25:09.982 -> 
20:25:09.982 -> OK
20:25:25.562 -> AT + HTTPPARA="URL","http://f0795047.xsph.ru/sim_message.php/?l=f0795047_bkn&w=652532"
20:25:25.630 -> 
20:25:25.630 -> OK
20:25:36.773 -> AT + HTTPACTION=0
20:25:36.773 -> 
20:25:36.773 -> OK
20:25:39.607 -> 
20:25:39.607 -> +HTTPACTION:0,200,67
20:25:53.043 -> AT + HTTPTERM
20:25:53.043 -> 
20:25:53.043 -> OK

*/
char AT[] = "AT";
char OK[] = "OK";
int onModulePin= 4;           // pin управления питанием SIM900
int onExtPower = 10;          // pin наличия внешнего питания (0-есть, 1-нет) берется с контактов реле
#define RST_PIN         6 //9          // RST RFID
#define SS_PIN          8 //10         // SS RFID
   // Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);  
   // создание объекта для связи с SIM900
SoftwareSerial gsm(3,2);      // RX, TX
INA219 monitorAccu;           // монитор аккумулятора

char response[MAX_LEN_BUFF];  // приемный буфер сообщения от SIM900
char curr_dt[18];             // текущее дата/время из SIM900
byte keyNum=0;                // номер ключа, с которого сняли с охраны/поставили на охрану
#define ARM_DISARM       0    // бит наблюдение/охрана в EEPROM
uint8_t currentState;         // битовые маски текущих состояний дверей, таблеток, питания  
#define DOOR_OPEN_CLOSE  0    // бит предыдущего состояния двери 0- была закрыта, 1- была открыта
#define DOOR_CHANGED     1    // бит произошла смена состояния двери
#define TO_ARM_CHANGE    2    // бит произошла смена состояния из наблюдения в охрану
#define TO_DISARM_CHANGE 3    // бит произошла смена состояния из охраны в наблюдение
#define CHANGE_STATE     4    // бит произошла смена какого-то состояния, необходимо отправить сообщение
#define BELL_ON          5    // бит идет звонок
#define GPRS_ON          6    // бит подключено к GPRS
#define ALARM            7    // бит тревога
uint8_t currentState2;        // битовые маски текущих состояний датчиков питания,температуры,воздуха
#define EXT_POWER        0    // бит наличия (0)/отсутствия (1) внешнего питания
#define NO_BELL          1    // отправка сообщения без звонка
#define SMS_TEST         2    // бит SMS-команды "test"
#define SMS_SARM         3    // бит SMS-команды "sarm" - set arm (постановка на охрану)
#define SMS_DARM         4    // бит SMS-команды "darm" - disarm (снятие с охраны)
#define PIR_ON           5    // бит датчика движения
#define SMS_ON           6    // разрешить дублирование сообщения по SMS
unsigned long doorTimeout;    // прошедшее время ожидания на открытие/закрытие двери
unsigned long armTimeout;     // прошедшее время ожидания на постановку/снятие с охраны
unsigned long bellTimeout;    // время начала звонка
unsigned long prev_mills;     // прошедшее время на проверку сигнала GPRS
unsigned long ligthTimeout;   // прошедшее время на смену светового сигнала
unsigned long pirTimeout;     // время задержки опроса датчика движения
unsigned long voltTimeout;    // время задержки опроса датчика тока на минимальное значение 3.5в
unsigned long iButtonTimeout; // время задержки опроса iButton
const unsigned long waitDoor = 60; // время ожидания (с) закрывания двери или ожидания снятия с охраны при открывании
const unsigned long waitBell = 20; // время ожидания (с) выполнения звонка
int pirPin = A0;               // Указываем пин датчика движения
int pirValue;                  // Переменная для сохранения значения из PIR (датчика движения)
float maxV,maxA;               // Значения тока и напряжения при смене режима заряд/разряд
float dht_tem=0.0,dht_hum=0.0; // усредненные значения температуры и влажности
unsigned int cntOnResetSIM;    // счетчик минут для выполнения периодического ежесуточного сброса SIM900
const uint8_t MAX_NO_GPRS = 40;// максимальное допустимое количество циклов отсутствия подключения к GPRS
uint8_t cntNotGPRS;            // счетчик циклов отсутствия GPRS, при достижении MAX_NO_GPRS - сброс SIM900
uint8_t cntAverage;            // счетчик чтения показаний DHT11 для усреднения
float sumTem=0.0, sumHum=0.0;  // сумма показаний датчика температуры и влажности для получения среднего
byte keyID[4];                 // буфер ID RFID 4 byte
byte isRFIDread=0;             // флаг чтения RFID
float volt,amper;

/*
* структура хранения настроек в EEPROM
*/
struct dt_EEPROM {
	byte arm;      // режим охрана/наблюдение
	char tel1[10]; // телефон 1
	char tel2[10]; // телефон 2
	byte key1[8];  // Button1
	byte key2[8];  // Button2
	byte key3[8];  // Button3
	char login[15];// Login
	char pswd[10]; // Password
	char host[40]; // http адрес f0795047.xsph.ru/sim_message.php
} bufEEPROM;
/*
* массив световых сигналов в единицах по 100 мс
* light[0] = {свет, пауза, свет, пауза} - нет GPRS
* light[1] = {свет, пауза, свет, пауза} - есть GPRS
* light[2] = {свет, пауза, свет, пауза} - постановка на охрану
* light[3] = {свет, пауза, свет, пауза} - режим охраны
*/
byte light [4][4] = 
{   {20, 5, 20, 5},
	{2, 3, 2, 20},
	{3, 3, 3, 3},
	{3, 30, 3 ,30}
};
byte curr_light=0; // текущий режим светового сигнала 0...3
byte curr_posL=0;  // текущая позиция свечения

//***************** зуки****************
void Sd_ReadOK() {  // звук ОК
  for (int i=400; i<6000; i=i*1.5) { tone(speakerPin, i); delay(20); }
  noTone(speakerPin);
}

void Sd_StartOK(){   // звук "Успешное включение"
  tone(speakerPin, NOTE_A7); delay(100);
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
* управление светодиодом и бузером - на контакторе около двери
*/
void lightOnOff () {
	if (millis() - ligthTimeout > light[curr_light][curr_posL] * 100) {
		if (curr_light!=2) digitalWrite(boozerPin, 0);
		switch (curr_posL) {
		case 0 : digitalWrite(B_Led, LOW); 
				if (curr_light==2) digitalWrite(boozerPin, 0); // только для постановки на охрану
				curr_posL = 1;
				break;
		case 1 : digitalWrite(B_Led, HIGH);  
				if (curr_light==2) digitalWrite(boozerPin, 1);
				curr_posL = 2;
				break;
		case 2 : digitalWrite(B_Led, LOW);  
				if (curr_light==2) digitalWrite(boozerPin, 0);
				curr_posL = 3;
				break;
		case 3 : digitalWrite(B_Led, HIGH);  
				if (curr_light==2) digitalWrite(boozerPin, 1);
				curr_posL = 0;
				break;
		}
		ligthTimeout = millis();
	}
}
// программное включение питания
// скопировано из D:\internet\Download\Arduino\Books\Arduino-Raspberry.pdf (стр.163)
void power_on ( )
{
  uint8_t answer=0;
  pinMode(onModulePin,OUTPUT);
  // checks if the module is started
  digitalWrite(onModulePin,LOW);
  delay(1000);
  digitalWrite(onModulePin,HIGH);
  delay(2000);
  digitalWrite(onModulePin,LOW);
  delay(5000);
  answer = sendATcommand(AT, OK, 2000);
  while (answer == 0) {
    digitalWrite(onModulePin,LOW);
    delay(1000);
    digitalWrite(onModulePin,HIGH);
    delay(2000);
    digitalWrite(onModulePin,LOW);
    delay(5000);
    // время ожидания ответа
    for (int i=0;i<5;i++) {
       if(answer == 0) {
         // Send АТ every two seconds and wait for the answer
         answer = sendATcommand (AT, OK, 5000) ;
       }
       else break;
    }
  }
}
// скопировано из
// D:\internet\Download\Arduino\Books\Arduino-Raspberry.pdf (стр.165)
// 
uint8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {
    uint8_t answer=0, x=0;
    unsigned long previos;
    
    memset(response, '\0', MAX_LEN_BUFF);    // Initialize the string
    
    while( gsm.available() > 0) gsm.readStringUntil('\n'); // Clean the input buffer
    gsm.println(ATcommand);
    x = 0;
    previos = millis();
    do
    {
        if(gsm.available() != 0)
        {
           // if there are data in the UART input buffer, reads it and checks for the asnwer
           if (x > MAX_LEN_BUFF-1) {
             Serial.println(response);
             Serial.print(F("Overflow input buffer=")); Serial.println(MAX_LEN_BUFF);
             answer=0;
             return answer;
           }
           response[x] = gsm.read();
           x++;   
           if (strstr(response, expected_answer) != NULL)
           {
             answer=1;
           }
        }
    } while (( answer == 0) && ( (millis () - previos) < timeout));
     Serial.print(response);
	 Serial.println(gsm.readStringUntil('\n')); // очистка приемного буфера до символа ВК
 return answer;
}
/*
* чтение настроек из EEPROM в буфер
*/
void readEEPROM () {
	EEPROM.get(0, bufEEPROM);    // прочитать из адреса 0
}
/*
* запись настроек из буфера в EEPROM 
*/
void writeEEPROM () {
	EEPROM.put(0, bufEEPROM);    // поместить в EEPROM по адресу 0
}
/*
* вывод на печать содежимого буфера EEPROM
*/
void outToSerial (byte *val, byte len, byte SYS) {
	for (byte i = 0; i < len; i++) {
		if (SYS == 0) Serial.print((char)val[i]);      // вывод char
	    else          Serial.print(val[i], SYS); 
		if (SYS == HEX) Serial.print(":");
	}
}
void printEEPROM () {
		Serial.println ();
		Serial.println (bufEEPROM.arm,BIN);
		outToSerial ((byte*)bufEEPROM.tel1, 10, DEC);
		Serial.println ();
		outToSerial ((byte*)bufEEPROM.tel2, 10, DEC);
		Serial.println ();
		outToSerial (bufEEPROM.key1, 8, HEX);
		Serial.println ();
		outToSerial (bufEEPROM.key2, 8, HEX);
		Serial.println ();
		outToSerial (bufEEPROM.key3, 8, HEX);
		Serial.println ();
		outToSerial ((byte*)bufEEPROM.login, 15, 0);
		Serial.println ();
		outToSerial ((byte*)bufEEPROM.pswd, 7, 0);
		Serial.println ();
		outToSerial ((byte*)bufEEPROM.host, 40, 0);
		Serial.println ();
}
boolean isDigital (String& snum) 
{
	for (uint8_t i=0;i<snum.length();i++)
	{
		if (!isDigit(snum[i])) return false;
	}
	return true;
}
/*
* передача на сайт сообщения
ждем ответа на AT+CREG?
+CREG: 1,5 или +CREG: 1,1
OK
at+cstt="GPB","",""  --- может быть ответ "+CME ERROR: operation not allowed"---
AT + SAPBR= 3,1, "CONTYPE", "GPRS"
AT + SAPBR = 1,1 --- может быть ответ "+CME ERROR: operation not allowed"---
AT + HTTPINIT
AT+ HTTPPARA="CID",1
AT + HTTPPARA="URL","http://f0795047.xsph.ru/sim_message.php/?l=f0795047_bkn&w=652532&y=23-04-02+14:00:00&b=99&a=none&r=disarm&d=close&p=ok&c=100&t=23&h=30&u=0&o=0&g=200"
AT + HTTPACTION=0
AT + HTTPTERM
*/
void sendMessage () {
	String host = "";
	char str[25]="";
	
	host = F("AT+CREG?");
	do {
		sendATcommand((char *)host.c_str(), OK, 2000);
		if (strstr(response, "1,5") != NULL or strstr(response, "1,1") != NULL )
			 break;
		 delay (10000);
    } while (true); // ждем подключения к GPRS
	
	host = F("AT+CCLK?");
	if (sendATcommand((char *)host.c_str(), OK, 2000) == 1) {
		parseTime ();
	}
	else {
		host = F("00.00.00+00:00:00");
		strcpy (curr_dt,host.c_str());
	}
//	host = F("at+cstt=\"GPB\",\"\",\"\"");                  // GPB Mobile
	host = F("at+cstt=\"internet\",\"\",\"\"");   // Megafon
	sendATcommand((char *)host.c_str(), OK, 2000);
	host = F("AT + SAPBR= 3,1, \"CONTYPE\", \"GPRS\"");
	sendATcommand((char *)host.c_str(), OK, 2000);
	host = F("AT + SAPBR = 1,1");
	sendATcommand((char *)host.c_str(), OK, 2000);
	host = F("AT + HTTPINIT");
	sendATcommand((char *)host.c_str(), OK, 2000);
	host = F("AT+ HTTPPARA=\"CID\",1");
	sendATcommand((char *)host.c_str(), OK, 2000);
	host = F("AT + HTTPPARA=\"URL\",\"http://");
	host += bufEEPROM.host;
	host += F("/?l=");
	host += bufEEPROM.login;
	host += F("&w=");
	host += bufEEPROM.pswd;
	host += F("&y=");
	host += curr_dt;
	host += F("&b=");
	host += itoa((int)keyNum,str,10);
	host += F("&a=");
	if(bit_is_set (currentState,ALARM)) host += F("alarm");
	else                                host += F("none");
	host += F("&r=");
	if (bit_is_set(bufEEPROM.arm,ARM_DISARM)) host += F("arm");
	else                                      host += F("disarm");
	host += F("&d=");
	if (digitalRead(Door)) host += F("open"); 
	else                   host += F("close");
	host += F("&s=");
	if (digitalRead(pirPin)) host += F("1"); 
	else                     host += F("0");
	host += F("&p=");
	if (bit_is_set(currentState2,EXT_POWER)) host += F("fault"); 
	else                                     host += F("ok");
	host += F("&c=");
	printFloat(maxV, 2, 7, &str[0]);
	strcat (str,"V");
	printFloat(maxA, 2, 9, &str[strlen(str)]);
	strcat (str,"mA");	
	maxV = monitorAccu.busVoltage();
	maxA = monitorAccu.shuntCurrent() * 1000;
	host += str;
	host += F("&t=");
	printFloat(dht_tem-5.5, 2, 7, &str[0]);
	host += str;
	host += F("&h=");
	printFloat(dht_hum-13, 2, 7, &str[0]);
	host += str;
	host += F("&u=0&o=0&g=20\"");
	sendATcommand((char *)host.c_str(), OK, 4000);
	host = F("AT + HTTPACTION=0");
	sendATcommand((char *)host.c_str(), (char *)F("+HTTPACTION:"), 10000);
	host = F("AT + HTTPTERM");
	sendATcommand((char *)host.c_str(), OK, 2000);
}
/*
* отправка SMS

AT+CMGF=1  -- установка текстового режима смс-сообщений
AT+CMGS="+79136513814" — отправка смс. После ввода команды 
выдает приглашение ">" после чего можно вводить текст сообщений. 
Завершается символом (0x1a) ESC или Ctrl-Z.

*/
void sendSMS () {
	String host = "";
	char str[25]="";
	char num[2] = "";	
	
	host = F("AT+CMGF=1");
	sendATcommand((char *)host.c_str(), OK, 1000);
	host = F("AT+CMGS=\"+7");
	for (byte i=0;i<10;i++) {
		itoa (bufEEPROM.tel1[i],num,10);
		host += num;
	}
	host += "\"";
	sendATcommand((char *)host.c_str(), OK, 1000);
	host = "";
	if(bit_is_set (currentState,ALARM)) host = F("alarm,");
	if (bit_is_set(bufEEPROM.arm,ARM_DISARM)) host += F("arm,");
	else                                      host += F("disarm,");
	if (bit_is_set(currentState,DOOR_OPEN_CLOSE)) host += F("door=open,"); 
	else                                          host += F("door=close,");
	if (bit_is_set(currentState2,PIR_ON)) host += F("sens=1,"); 
	else                                  host += F("sens=0,");
	host += F("accu=");
	printFloat(maxV, 2, 7, &str[0]);
	strcat (str,"V");
	printFloat(maxA, 2, 9, &str[strlen(str)]);
	strcat (str,"mA");	
	host += str;
	host += F("t=");
	printFloat(dht_tem-5.5, 2, 7, &str[0]);
	host += str;
	host += F("h=");
	printFloat(dht_hum-13, 2, 7, &str[0]);
	host += str;
	//host += "\"";
	host += "\x1a";
	sendATcommand((char *)host.c_str(), OK, 5000);
}
/*
* Выполнить звонок
*/
void bellOn () {
	String host = "";
	char num[2] = "";
	
	host = F("ATD+ +7");
	for (byte i=0;i<10;i++) {
		itoa (bufEEPROM.tel1[i],num,10);
		host += num;
	}
	host += ";";
	sendATcommand((char *)host.c_str(), OK, 1000);
    bellTimeout = millis ();                   // запускаем отсчет таймаута звонка	
}
/*
* Прекратить звонок (повесить трубку)
*/
void bellOff () {
	String host = "";
	
	host = F("ATH");
	sendATcommand((char *)host.c_str(), OK, 500);
	
}
/*
* запись в EEPROM ключей RFID
*/
void storeRFID (byte num) {
	byte cnt=0;
	// опрос RFID
	iButtonTimeout = millis ();
	Serial.println ();
	Serial.print (F("Please present RFID within 10 c"));
	while (cnt < 10) {
		if ((millis () - iButtonTimeout) > 1000) {
			cnt++;
			Serial.print (".");
			iButtonTimeout = millis ();
		}
		if (mfrc522.PICC_IsNewCardPresent()) {
			// Select one of the cards
			if ( mfrc522.PICC_ReadCardSerial()) {
				memcpy (keyID, mfrc522.uid.uidByte,4);
				Serial.println ();
				for (int i =0;i<4;i++)  {
					Serial.print ((byte)(mfrc522.uid.uidByte[i]),HEX);
					Serial.print (":");
				}
				switch (num) {
					case 1 :
						memset(bufEEPROM.key1,0, sizeof (bufEEPROM.key1));
						memcpy(bufEEPROM.key1, keyID,4);
						break;
					case 2 :
						memset(bufEEPROM.key2,0, sizeof (bufEEPROM.key2));
						memcpy(bufEEPROM.key2, keyID,4);
						break;
					case 3 :
						memset(bufEEPROM.key3,0, sizeof (bufEEPROM.key3));
						memcpy(bufEEPROM.key3, keyID,4);
						break;
				}
				Serial.println (F("RFID stored"));
				Sd_ReadOK ();
				return;
			}
		}
	}
	Serial.println (F("RFID NOT presented."));
}
/*
 *  получение команды из монитора последовательного порта
*/
String inputString = ""; // строка, собираемая из данных, приходящих в последовательный порт
byte f_init = 0;           // флаг, если была команда init, то = 1
void serialEvent()
{
  String cmd;
  String s_num;
  
  while (Serial.available())
  { // получить очередной байт:
    char inChar = (char)Serial.read();
    // добавить в строку
    if (inChar != '\n') inputString += inChar;
    else  // /n - конец передачи
    {
      Serial.println ();
      Serial.println(inputString);
      if (inputString[0] == '@'){
        gsm.write(0x1a);  // передача в SIM900 символа окончания сообщения
      }
	  else {
          cmd = inputString.substring (0,2); // выделяем первые 2 символа
		  if (cmd == "at" or cmd == "AT") {
			  // поступила команда для SIM900
            if (inputString.length() > MAX_LEN_BUFF-1) {
                Serial.println(inputString);
                Serial.print(F("Overflow input console buffer=")); Serial.println(MAX_LEN_BUFF);
                Serial.println(F("Ignore input !"));
            }
            else {
                //inputString.toCharArray(buff, inputString.length() + 1);
                //gsm.write(buff);
				gsm.write(inputString.c_str());
            }
            gsm.write('\n');
		  }
		  else {
              cmd = inputString.substring (0,4); // выделяем первые 4 символа
			  if (cmd=="init"){ 				 // команда инициализации и чтения настроек из EEPROM в буфер
				readEEPROM ();
				bitClear (bufEEPROM.arm,ARM_DISARM);      // сбросить 0-бит в режим наблюдения
				printEEPROM ();
				f_init = 1;
              }
              else if (cmd=="tel1") {// команда записи в буфер телефона №1
			    parseTel ();
              }
              else if (cmd=="tel2") {// команда записи в буфер телефона №2
			    parseTel ();
              }
			  else if (cmd=="stop") { // запись настроек из буфера в EEPROM
			     if (f_init == 1) {
			        writeEEPROM ();
					f_init =0;
				 }
			     else 
					 Serial.println (F("Not previos command init"));
 			  }
              else if (cmd=="time") {// команда чтения даты/времени из SIM900
			    if (sendATcommand((char *)F("AT+CCLK?"), OK, 2000) == 1) {
					parseTime ();
					Serial.println (curr_dt);
				}
				// Для установки времени используется АТ-команда
				// AT+CCLK=«yy/mm/dd,hh:mm:ss+zz» 
				// Обязательно в таком формате и с ведущими нулями.
              }
              else if (cmd=="logn") {// команда записи в SIM900 логина
			    memset(bufEEPROM.login,'\0', sizeof (bufEEPROM.login));
				memcpy(bufEEPROM.login, &inputString[5],strlen(&inputString[5])-1);
				Serial.println (bufEEPROM.login);
              }
              else if (cmd=="pswd") {// команда записи в SIM900 пароля
			    memset(bufEEPROM.pswd,'\0', sizeof (bufEEPROM.pswd));
				memcpy(bufEEPROM.pswd, &inputString[5],strlen(&inputString[5])-1);
				Serial.println (bufEEPROM.pswd);
              }
              else if (cmd=="host") {// команда записи в SIM900 адреса хоста
			    memset(bufEEPROM.host,'\0', sizeof (bufEEPROM.host));
				memcpy(bufEEPROM.host, &inputString[5],strlen(&inputString[5])-1);
				Serial.println (bufEEPROM.host);
              }
              else if (cmd=="btn1" or cmd=="btn2" or cmd=="btn3") {// команда записи в SIM900 iButton 1,2,3
				  Serial.println (cmd);
				  byte num=0;
				  char snum[2] = "0";
				  snum[0] = cmd [3];
				  num = (byte)atoi (snum);
				  storeRFID (num);
				  //storeCyfral (num);
              }
              else if (cmd=="test") {// команда передачи тестового сообщения на сервер
				bitSet (currentState,CHANGE_STATE);      // отправить сообщение
              }
             else if (cmd=="rest") {// команда сброса питапния SIM900
				power_on();
              }
             else if (cmd=="rfid") {// команда сброса RFID
				mfrc522.PCD_Init();		
              }
			  else {
				  Serial.println (F("Unknown command."));
			  }
		  }
	  }
      inputString = "";
	}
  }
}
/*
* выделяет строку с датой/временем из буфера response
* AT+CCLK?  Вернет текущее время в виде: +CCLK: "11/04/11,01:46:33+00"
*/
void parseTime () {
	memset(curr_dt, '\0', 18);    // Initialize the string
	char mark = '"';
	char *cclk;
	cclk = strchr (response,mark) + 1;
	if (cclk != NULL) {
		for (uint8_t i=0;i<17;i++) {
			curr_dt[i] = cclk[i];
		}
		curr_dt[2] = '.';
		curr_dt[5] = '.';
		curr_dt[8] = '+';
	}
}
/*
* Выделяет из команды 10 цифр телефонного номера и записывает его в буфер EEPROM
*/
void parseTel () {
	String s_num;
	char str[2]="";
	int num;
	str[0] = inputString[3];  //выделяем порядковый номер телефона 1 или 2 из команды "tel1" "tel2"
	num = atoi (str);
    s_num = inputString.substring (5); // выделяем собственно номер телефона
	s_num.trim();
	if (isDigital(s_num) and s_num.length() == 10) {
/*		
		switch (num) {
		   case 1 : memcpy (bufEEPROM.tel1,s_num.c_str(),10);
		   break;
		   case 2 : memcpy (bufEEPROM.tel2,s_num.c_str(),10);
		   break;
		}
*/		
		for (byte i=0;i<10;i++) {
			str[0] = s_num[i];
			switch (num) {
			   case 1 : bufEEPROM.tel1[i] = (byte)atoi (str);
			   break;
			   case 2 : bufEEPROM.tel2[i] = (byte)atoi (str);
			   break;
			}
		}
	}
	else Serial.println (F("telN=XXXXXXXXXX - 10 digit")); //9136513814
  }
/*
 проверка входящих SMS
at+cmgr=1,0   -- запрос текста SMS
              -- ответ на запрос текста SMS
+CMGR: "REC UNREAD","+79131234567","","23/02/21,16:09:33+28"
Test

OK
---------------------------------------------------------
at+cmgd=1,0    -- удаление SMS номер 1

OK

*/
uint8_t tstSMS () {
	char tel[11];
	uint8_t ownTel=0; // свой телефон (1,2) или чужой (0)
	
	sendATcommand((char*)"AT+CMGR=1,0", OK, 1000);
	if (strstr(response, (char*)"+CMGR:") != NULL) {
		// пришла SMS
		memcpy (tel,bufEEPROM.tel1,10); tel[10]='\0';
		for (byte i=0;i<10;i++) tel[i] += 48; // преобразование числа в символ
		if (strstr(response, tel) != NULL) {
			ownTel = 1;
		}
		else {
			memcpy (tel,bufEEPROM.tel2,10); tel[10]='\0';
		    for (byte i=0;i<10;i++) tel[i] += 48; // преобразование числа в символ
			if (strstr(response, tel) != NULL) {
				ownTel = 2;
			}
		}
		return ownTel;
	}
	return 0;
}
/*
* опрос RFID 
*/
void readRFID () {

	isRFIDread=0;
	if ( (millis () - iButtonTimeout)>1000 ) {
		if (mfrc522.PICC_IsNewCardPresent()) {
			// Select one of the cards
			if ( mfrc522.PICC_ReadCardSerial()) {
				memcpy (keyID, mfrc522.uid.uidByte,4);
				isRFIDread=1;
				Serial.println ();
				for (int i =0;i<4;i++)  {
					Serial.print ((byte)(mfrc522.uid.uidByte[i]),HEX);
					Serial.print (":");
				}
			}
		}
		iButtonTimeout = millis ();	// следующий опрос RFID чере 1 с
	}
	//если ключ есть в памяти, то меняем режим охрана/наблюдение
	if (isRFIDread==1) {
		if      (!memcmp (keyID,bufEEPROM.key1,4)) keyNum=1;
		else if (!memcmp (keyID,bufEEPROM.key2,4)) keyNum=2;
		else if (!memcmp (keyID,bufEEPROM.key3,4)) keyNum=3;
		else keyNum=99;
		
		if (keyNum > 0 and keyNum < 4 ) {
			Sd_Button();                              // звук "распознан iButton"
		   if (bit_is_clear(bufEEPROM.arm,ARM_DISARM)) { // был режим наблюдения?
			bitSet (currentState,TO_ARM_CHANGE);      // начинаем процесс постановки на охрану
			bitClear (currentState,TO_DISARM_CHANGE); // прерываем процесс снятия с охраны
			armTimeout = millis ();                   // запускаем отсчет таймаута постановки
			curr_light=2;                             // текущий режим светового сигнала 2 - постановка 
			iButtonTimeout = millis ();				  // таймаут на повторное срабатывание iButton
		   }
		   else {
			bitSet (currentState,TO_DISARM_CHANGE); // начинаем процесс снятия с охраны
			bitClear (currentState,TO_ARM_CHANGE);  // прерываем процесс постановки на охрану
			armTimeout = millis ();                 // запускаем отсчет таймаута снятия
			iButtonTimeout = millis ();				// таймаут на повторное срабатывание iButton
		   }
		}
		
	}	
}
/*
* проверяем состояние дверей
*/
void checkDoor () {
	if (digitalRead(Door)==0 and bit_is_set(currentState,DOOR_OPEN_CLOSE) and bit_is_set(bufEEPROM.arm,ARM_DISARM)) {
		//дверь сейчас закрыта, но была открыта
		bitClear (currentState,DOOR_OPEN_CLOSE);
		bitSet (currentState,DOOR_CHANGED);   //фиксация изменения состояния двери
		doorTimeout = millis ();              // запускаем отсчет таймаута
	}
	else if (digitalRead(Door)==1 and bit_is_clear(currentState,DOOR_OPEN_CLOSE) and bit_is_set(bufEEPROM.arm,ARM_DISARM)) {
		//дверь открыта впервые
		bitSet (currentState,DOOR_OPEN_CLOSE);
		bitSet (currentState,DOOR_CHANGED);          // фиксация изменения состояния двери
		if (bit_is_clear(currentState,TO_ARM_CHANGE))// не была начата постановка на охрану
			doorTimeout = millis (); // - waitDoor*1000; // имитируем истечение таймаута для оповещения
		else
			doorTimeout = millis ();                 // запускаем отсчет таймаута
	}
	else if (digitalRead(Door)==1 and bit_is_set(currentState,DOOR_OPEN_CLOSE) and bit_is_set(bufEEPROM.arm,ARM_DISARM)) {
		//дверь продолжает быть открыта
		if ((millis () - doorTimeout) > waitDoor*1000) {
			bitSet (currentState,DOOR_CHANGED);   // фиксация изменения состояния двери
			doorTimeout = millis ();              // запускаем новый отсчет таймаута
		}
	}
	else if (digitalRead(Door)==1 and bit_is_clear(bufEEPROM.arm,ARM_DISARM)) {
		curr_light=2;                             // текущий режим светового сигнала 2 - дверь открыта
	}
	
}
/*
* Считываем значение от датчика движения
*/
void readSensor () {
   if (digitalRead(pirPin) == 1) {               // Если движение есть
        if(bit_is_clear(currentState2,PIR_ON)) {
			// раньше движения не было и прошло больше 20 сек после ранее зафиксированного движения
			if ((millis () - pirTimeout > 20000) and bit_is_set(bufEEPROM.arm,ARM_DISARM)) {
				bitSet (currentState2,PIR_ON);
				bitSet (currentState,ALARM);             // тревога
				bitSet (currentState,CHANGE_STATE);      // отправить сообщение
				pirTimeout = millis ();
			}				
		}
    }
	else { // если движения нет в течение 60 сек
		if ((millis () - pirTimeout > 60000) ) {
			bitClear (currentState2,PIR_ON);
		}
	}
}
/* 
* проверяем таймауты таблеток
*/
void chectTimeOutKey () {
	if (bit_is_set (currentState,TO_ARM_CHANGE) and ((millis () - armTimeout) >= waitDoor*1000)) {
		// истекло время постановки на охрану
		bitSet (bufEEPROM.arm,ARM_DISARM);        // переключить 0-бит в режим охраны
		bitClear (currentState,TO_ARM_CHANGE);    // завершаем процесс постановки на охрану
		curr_light=3;                             // текущий режим светового сигнала 3 - режим охраны
		if (digitalRead(Door)==1) {               // дверь открыта
			bitSet (currentState,ALARM);          // тревога
		}
		bitClear (currentState2,PIR_ON);          // очистить событие датчика движения		
		bitSet (currentState,CHANGE_STATE);       // отправить сообщение
		printEEPROM ();
		EEPROM.put(0, bufEEPROM.arm);             // поместить в EEPROM по адресу 0
	}
	if (bit_is_set (currentState,TO_DISARM_CHANGE) and ((millis () - armTimeout) >= 1000)) {
		// прошел таймаут на снятие с охраны
		bitClear(bufEEPROM.arm,ARM_DISARM);       // переключить 0-бит в режим наблюдения
		bitClear (currentState,TO_DISARM_CHANGE); // завершаем процесс снятия с охраны
		bitClear (currentState,DOOR_CHANGED);     // обнуляем смену состояния двери
		bitSet (currentState,CHANGE_STATE);       // отправить сообщение
		printEEPROM ();
		EEPROM.put(0, bufEEPROM.arm);             // поместить в EEPROM по адресу 0
		curr_light=1;                             // текущий режим светового сигнала 1 - есть GPRS
	}
}	
/*
* проверяем таймауты дверей
*/
void checkTimeOutDoor () {
	if (bit_is_set (currentState,DOOR_CHANGED) and ((millis () - doorTimeout) >= waitDoor*1000)) {
		// прошел таймаут на изменение состояния двери
		bitClear (currentState,DOOR_CHANGED);     // сбрасываем изменение состояния дверей
		if ((digitalRead(Door)==1) and (bit_is_set(bufEEPROM.arm,ARM_DISARM))) {    
		    // дверь открыта и был установлен режим охраны
			bitSet (currentState,ALARM);             // тревога
			bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		}
		else if ((digitalRead(Door)==0) and (bit_is_set(bufEEPROM.arm,ARM_DISARM))) {
			// дверь закрыта и завершена постановка в режим охраны
			bitClear (currentState,ALARM);           // сбрасываем режим тревоги
			bitClear (currentState,DOOR_OPEN_CLOSE); // запоминаем состояние дверь закрыта
		}
		else if ((digitalRead(Door)==1) and (bit_is_clear(bufEEPROM.arm,ARM_DISARM))) {
			// дверь открыта и был режим наблюдения
			bitClear (currentState,ALARM);           // сбрасываем режим тревоги
			bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		}
		else if ((digitalRead(Door)==0) and (bit_is_clear(bufEEPROM.arm,ARM_DISARM))) {
			// дверь успели закрыть в течение таймаута и был режим наблюдения
			bitClear (currentState,ALARM);           // сбрасываем режим тревоги
			bitClear (currentState,DOOR_OPEN_CLOSE); // запоминаем состояние дверь закрыта
			bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		}
		else if ((digitalRead(Door)==0) and (bit_is_set(currentState,DOOR_OPEN_CLOSE))) {
			// дверь успели закрыть в течение таймаута 
			bitClear (currentState,DOOR_OPEN_CLOSE); // запоминаем состояние дверь закрыта
			bitClear (currentState,ALARM);           // сбрасываем режим тревоги
			bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		}
	}
}
/*
* проверяем наличие внешнего питания
*/
void checkExtPower () {
	if (digitalRead (onExtPower) == 1) {
		// отключено внешнее питание 
		if (bit_is_clear(currentState2,EXT_POWER)) {
			// ранее внешнее питание было включено
			bitSet(currentState2,EXT_POWER);         // запоминаем, что питание отключено
			bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		}
    }
	else {
		// включено внешнее питание 
		if (bit_is_set(currentState2,EXT_POWER)) {
			// ранее внешнее питание было отключено
			bitClear(currentState2,EXT_POWER);       // запоминаем, что питание включено
			bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		}
	}
}
/*
* установка режима свечения светодиода
*/
void setModeLight () {
 if(bit_is_set(currentState,GPRS_ON)) {
	  if (bit_is_set(bufEEPROM.arm,ARM_DISARM))
		curr_light=3; // текущий режим светового сигнала 3 - режим охраны
	  else if (bit_is_set(currentState,TO_ARM_CHANGE))
		curr_light=2; // текущий режим светового сигнала 2 - постановка на охрану
	  else if (digitalRead(Door)==0)
		curr_light=1; // текущий режим светового сигнала 1 - режим наблюдения, есть GPRS
  }
  else {
	curr_light=0; // текущий режим светового сигнала 0 - нет GPRS 
  }
}
/*
* отработка директивы из SMS
*/
void eventSMS () {
	if (strstr(response, "test") != NULL) {      // текущее состояние
		bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		//bitSet (currentState2,NO_BELL);
	}
	else if (strstr(response, "sarm") != NULL) { // постановка на охрану
		bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		bitSet (bufEEPROM.arm,ARM_DISARM);       // переключить 0-бит в режим охраны
		curr_light=3;                            // текущий режим светового сигнала 3 - режим охраны
		EEPROM.put(0, bufEEPROM.arm);            // поместить в EEPROM по адресу 0
		Sd_Button();                             // звук "распознан iButton"
	}
	else if (strstr(response, "darm") != NULL) { // снятие с охраны
		bitSet (currentState,CHANGE_STATE);      // отправить сообщение
		bitClear(bufEEPROM.arm,ARM_DISARM);       // переключить 0-бит в режим наблюдения
		EEPROM.put(0, bufEEPROM.arm);             // поместить в EEPROM по адресу 0
		Sd_Button();                              // звук "распознан iButton"
		curr_light=1;                             // текущий режим светового сигнала 1 - есть GPRS
	}
	else if (strstr(response, "rest") != NULL) {  // сброс модуля SIM900
		keyNum += 200;
		bitSet (currentState,CHANGE_STATE);       // отправить сообщение
		bitSet (currentState2,NO_BELL);           // без звонка
		power_on ();                              // сброс модуля SIM900
		mfrc522.PCD_Init();		                  // Сброс модуля RFID (Init MFRC522)
		cntOnResetSIM = 0;
		//asm volatile("jmp 0x00");                 // reboot.... НЕ РАБОТАЕТ !!!
	}
	else if (strstr(response, "esms") != NULL) {  // разрешить дублиирование сообщения по SMS
	    bitSet (currentState2,SMS_ON);
	}
	else if (strstr(response, "dsms") != NULL) {  // запретить дублиирование сообщения по SMS
	    bitClear (currentState2,SMS_ON);
	}
}
/*
* читаем показания датчика влажности и температуры и усредняем значения
*/
void readHumTemp () {
	if(dht.read()==DHT_OK){
		cntAverage++;
		if(cntAverage>=200) {
			cntAverage = 101;
			sumHum = dht_hum * 100;
			sumTem = dht_tem * 100;
		}
		sumHum += dht.hum;
		sumTem += dht.tem;
		dht_hum = sumHum / cntAverage;
		dht_tem = sumTem / cntAverage;
	}
	else {		
	    dht_tem = 0.0;  
		dht_hum = 0.0;		
	}
}
/*
* читаем показания датчика тока и напряжения
*/
void readVoltAmper () {
	volt = monitorAccu.busVoltage();
	amper = monitorAccu.shuntCurrent() * 1000;
	if (volt < 3.8 or amper > 1000.0) {
		if (millis () - voltTimeout > 30000) {
			bitSet (currentState,CHANGE_STATE);      // отправить сообщение
			//bitSet (currentState2,NO_BELL);
			voltTimeout = millis ();
		}
	}
	else {
		// запоминаем текущще значения
		maxV = volt;
		maxA = amper;
		voltTimeout = millis ();
	}
}
void setup() {
   pinMode(onModulePin, OUTPUT);          // пин управления питанием SIM900
   pinMode(onExtPower, INPUT_PULLUP);     // датчик внешнего питания (0-есть, 1 -нет)
   pinMode(pirPin, INPUT);                // Установка пин датчика движения как вход
   pinMode(boozerPin, OUTPUT);
   digitalWrite(boozerPin, 1);
   delay(600);
   digitalWrite(boozerPin, 0);
   Serial.begin(9600);

   gsm.begin(9600);
   
   monitorAccu.begin();

	SPI.begin();			// Init SPI bus for RFID
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	//mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details


   Serial.println(F("Start"));
   Serial.println(MAX_LEN_BUFF);          // размер буфера SoftwareSerial
   
   readEEPROM ();
   f_init = 1;                            // флаг загрузки данных из EEPROM в буфер
   printEEPROM ();
   Serial.println (F("Wait 15c...."));
   delay (15000);  // задержка для возможности загрузки нового скетча при зависании
   
  pinMode(speakerPin, OUTPUT);
  pinMode(Door, INPUT_PULLUP);            // включаем чтение и подтягиваем датчик двери к +5В
  currentState = 0;                       // очищаем все маски событий
  currentState2 = 0;                      // очищаем все маски событий
  
  pinMode(B_Led, OUTPUT);                 // RGB-led
  digitalWrite(B_Led, HIGH);              // внутренний LED на плате ардуино и внешний на контакторе (BKN)
  curr_light=0;                           // текущий режим светового сигнала 0 - нет GPRS
  curr_posL=0;                            // текущая позиция свечения
  ligthTimeout = millis();                // старт таймаута на отработку светового сигнала
  Sd_StartOK();                           // звук запуска программы
  
  keyNum=100;                             // номер ключа, с которого сняли с охраны/поставили на охрану
                                          // при рестарте программы значение 100, чтобы идентифицировать
										  // рестарт в записи на сайте
  bitSet (currentState,CHANGE_STATE);     // отправить сообщение о перезапуске устройства
  bitSet (currentState2,NO_BELL);		  // без звонка

   // проверяем SIM900 уже запущен ?
   if (sendATcommand(AT, OK, 4000) == 0) {
      power_on ();
   }
      prev_mills = millis()+60000;
	pirTimeout = millis()+60000;
	
	// читаем показания датчика тока и напряжения
	maxV = monitorAccu.busVoltage();
	maxA = monitorAccu.shuntCurrent() * 1000;
	
	cntOnResetSIM = 0;                     // счетчик минут до сброса SIM900
	iButtonTimeout = millis ()+3000;	   // таймаут на повторное срабатывание iButton
	
	sumHum = 0;
	sumTem = 0;
	cntAverage = 0;
}  

void loop() {


    serialEvent();  // чтение символа из монитора порта
	lightOnOff ();  // отработка светового сигнала
	
    if (gsm.available()) {
     // вывод в монитор порта сообщения из SIM900
       Serial.write(gsm.read());
    }
	
	   // прошла 1 минута
    if ((millis () - prev_mills > 60000) ) {  //or (bit_is_clear(currentState,GPRS_ON))
			prev_mills = millis ();
			cntOnResetSIM++;
			if (cntOnResetSIM > 360) {                       // 6 часов
				cntOnResetSIM = 0;
				power_on ();                                 // сброс SIM900 4 раз в сутки
				keyNum=255;
				bitSet (currentState,CHANGE_STATE);          // отправить сообщение
				bitSet (currentState2,NO_BELL);
			}
			
			mfrc522.PCD_Init();		// Init MFRC522 (часто зависает, поэтому приходиться сбрасывать)
			delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
			
			bitClear (currentState,GPRS_ON);
			// проверяем подключение к GPRS 
			uint8_t gprs = sendATcommand((char*)"AT+CREG?", OK, 2000);
			if (!gprs) {
				power_on ();            // мягкий сброс модуля SIM900
				cntOnResetSIM = 0;
			}
			else if (strstr(response, "1,5") != NULL or strstr(response, "1,1") != NULL ) {
				bitSet(currentState,GPRS_ON);
				cntNotGPRS = 0;
			}
			else if (cntNotGPRS >= MAX_NO_GPRS) {
				power_on ();                                 // сброс SIM900
				cntNotGPRS = 0;
			}
			else cntNotGPRS++;
			
			if(bit_is_set(currentState,GPRS_ON)) {
				// проверяем поступление SMS
				uint8_t telNum = 10 + tstSMS ();
				if (telNum>10) {
					keyNum = telNum;   // идентификатор номера ячейки EEPROM(+10) с телефоном, отправившего смс
					eventSMS ();       // отработка директивы из SMS
				}
					  // удалить SMS
				sendATcommand((char*)"AT+CMGD=1,0", OK, 1000);
			}
			
			// читаем показания датчика тока, напряжения, влажности и температуры
			readVoltAmper ();
			readHumTemp ();
			
			// печатаем показания датчика тока, напряжения, влажности и температуры
			Serial.println((String)"V="+maxV+" mA="+maxA);
			Serial.println((String)"door="+digitalRead(Door)+" sens="+digitalRead(pirPin));
			Serial.println((String)"вл-ть: " + (dht_hum-13) + "% темп: " + (dht_tem-5.5) + "/"+dht.tem); 
    }
	// Считываем значение от датчика движения
	readSensor ();
 	
	// опрос RFID
	readRFID ();
	
	// проверяем состояние дверей
	checkDoor ();
	
	// проверяем таймауты таблеток
	chectTimeOutKey ();		  
	          
	// проверяем таймауты дверей
	checkTimeOutDoor ();
	     
	// проверяем наличие внешнего питания
	checkExtPower ();
	
	// установка режима свечения светодиода
	setModeLight ();
	
 	          // отправка сообщения при смене состояний
	if (bit_is_set (currentState,CHANGE_STATE)) {
		sendMessage ();							  // отправка сообщения на сервер
		if (bit_is_set (currentState2,SMS_ON)) {
			sendSMS();                            // отправка по SMS 
		}
		bitClear (currentState,CHANGE_STATE);     // сброс состояния
		bitClear (currentState,ALARM);            // сбрасываем режим тревоги
		keyNum=0;                                 // обнуляем номер ключа
		
		if (bit_is_clear (currentState2,NO_BELL) and bit_is_clear (currentState,BELL_ON)) {    
		  // звонок в данный момент не выполняется, но необходимо позвонить
			bitSet (currentState,BELL_ON);            // установка флага звонка 
			bellOn ();                                // выполнение звонка
		}
		bitClear (currentState2,NO_BELL); 
	}
	         // выполнется звонок
	if (bit_is_set (currentState,BELL_ON)) {
		if ((millis () - bellTimeout) > waitBell*1000) {
			// истекло время звонка
		   bitClear (currentState,BELL_ON);        // сброс флага звонка
		   bellOff ();                             // завершить звонок
		}
	}
 }
/*
Скетч использует 25962 байт (84%) памяти устройства. Всего доступно 30720 байт.
Глобальные переменные используют 1484 байт (72%) динамической памяти, оставляя 564 байт для локальных переменных. Максимум: 2048 байт.
*/