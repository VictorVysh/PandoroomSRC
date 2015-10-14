#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN		9
#define SS_PIN		10

//EA 82 E4 CD - Serial card
//int CardCode[] = {0xEA,0x82,0xE4,0xCD};//Правильный код карты

int CardCode[] = {83,93,107,129,0,82,128};//Фиолетовая
//int CardCode[] = {83,69,58,131,0,242,128};//Голубая
//int CardCode[] = {247,109,46,59};//Оранжевая

#define WriteCardPort 8

#define Card_is_correct Serial.println("Correct card");digitalWrite(8,HIGH);delay(500);
#define Card_is_not_correct Serial.println("Not correct card");digitalWrite(8,LOW);

MFRC522 mfrc522(SS_PIN, RST_PIN);//Подключение библиотеки для работы с модулем

void setup() 
{
  Serial.begin(9600);//Инициализация uart
  while (!Serial);//Пока не инициализировано, ждем.
  SPI.begin();//Включаем SPI
  mfrc522.PCD_Init();//Включаем модуль
  pinMode(8, OUTPUT);
}

void loop() 
{
  if(CheckCard()==1)//Если мы нашли верную карту, то
  {
     Card_is_correct;//Выполняем действие заданное в define
  }
  else//Если карта не верная
  {
    Card_is_not_correct;//Выполняем действие заданное в define
  }
}

int CheckCard()
{
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())//Смотрим, есть ли карта.
  {
     for (byte i = 0; i < mfrc522.uid.size; i++)//Проверка кода карты.
     {
        Serial.println(mfrc522.uid.uidByte[i]);
        if(mfrc522.uid.uidByte[i]==CardCode[i])//Если цифра совпадает, то ок.
        {
          //Ничего не делаем
        }
        else
        {//Если нет, то возвращаем 0
          return 0;
        }
     }
     //Делаем сброс и инициализацию
     mfrc522.PCD_Reset();
     mfrc522.PCD_Init();
     return 1;//Возвращаем, что результат верный
  }
  else
  {
      return 0;//Карта не найдена.
  }
}
