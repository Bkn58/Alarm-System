/*
* Преобразование в строку числа с плавающей точкой
  за основу взят листинг функции size_t Print::printFloat(double number, uint8_t digits) 
  с сайта https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Print.cpp
  
  double number  - исходное число с плавающей точкой
  uint8_t digits - количество знаков после запятой
  uint8_t buflen - общее количество знаков для отображения числа
  char* buf      - указатель на массив символов, в который будет помещено печатаемое число
  
  возвращает количество напечатанных символов
*/
size_t printFloat(double number, uint8_t digits, uint8_t buflen, char* buf) 
{ 
  size_t n = 0;
  buf[0]=0;
  char str[10];
  str[0] = 0;
  
  if (buflen > 3) {
	if (isnan(number)) {buf = "nan"; return 3;}
	if (isinf(number)) {buf = "inf"; return 3;}
	if (number > 4294967040.0) {buf = "ovf"; return 3;}  // constant determined empirically
	if (number <-4294967040.0) {buf = "ovf"; return 3;}  // constant determined empirically
  }
  
  // Handle negative numbers
  if (number < 0.0)
  {
     strcat (buf,"-");
	 n++;
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;
  
  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  ltoa (int_part, str, 10);
  if (strlen(buf)+strlen(str) < buflen) strcat (buf, str);
  n += strlen(str);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
	  if (strlen(buf)+1 < buflen) {
		  strcat(buf,".");
		  n++; 
	  }
  }

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    unsigned int toPrint = (unsigned int)(remainder);
	itoa (toPrint, str, 10);
	if (strlen(buf)+strlen(str) < buflen) {
		strcat (buf, str);
		n += strlen(str);
	}
    remainder -= toPrint; 
  } 
  
  return n;
}
