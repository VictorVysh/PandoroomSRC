/*
 * Attiny2313_Password_Telephone.c
 *
 * Created: 13.10.2015 13:23:33
 *  Author: Vic
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>

/*==========Порт вывода==========*/
//Порт вывода
#define OUT_DDR DDRD
#define OUT_PORT PORTD
#define OUT_SIG_PIN PD0
//Инициализация
#define INIT_OUT_PORT OUT_DDR = _BV(OUT_SIG_PIN);
//Быстрое переключение
#define OUT_SIG_ON OUT_PORT |= _BV(OUT_SIG_PIN)
#define OUT_SIG_OFF OUT_PORT &= ~_BV(OUT_SIG_PIN)
/*==========Порт вывода==========*/

/*==========Кнопки==========*/
//Порты ввода
#define BTNS_DDR DDRB
#define BTNS_PORT PORTB
#define BTNS_PIN PINB

//Правильные кнопки
#define BTN1 PB0 //2
#define BTN2 PB1 //3
#define BTN3 PB2 //4
#define BTN4 PB3 //8
#define BTN5 PB4 //9
//Не правильная кнопка
#define BTN6 PB5 // "Лишняя"

#define SET_BTNS_ON _BV(BTN1)|_BV(BTN2)|_BV(BTN3)|_BV(BTN4)|_BV(BTN5)|_BV(BTN6);//Установка в положение HIGH
/*==========Кнопки==========*/

//Массив кнопок
#define MAXBTN 6//Длинна пароля
//int CorrectButtons[MAXBTN] = {BTN3,BTN2,BTN1,BTN2,BTN5,BTN4};//Пароль 431375 (Лифт)
int CorrectButtons[MAXBTN] = {BTN3,BTN4,BTN2,BTN4,BTN1,BTN5};//Пароль 483829 (1408)
int NotCorrectButton = BTN6;//А это "лишняя" кнопка
int ToCheckNow=0;//Кнопка, которую мы сейчас проверяем
//Функция инициализации кнопок на вход
void InitInput();
//Функция чтения кнопок
void ReadBtns();
//Функция обработки нажатия кнопок
void ReadHandle();

int main(void)
{
	InitInput();//Инициализируем кнопки
	INIT_OUT_PORT;//Инициализируем выход
    while(1)
    {
		ReadHandle();//Обработка ввода
    }
}

void ReadHandle()
{
	if(ToCheckNow==MAXBTN)
	{
		OUT_SIG_ON;
		for(int i=0;i<15;i++){_delay_ms(100);}
		OUT_SIG_OFF;
		ToCheckNow=0;
	}
	else
	{
		ReadBtns();
	}
}

void InitInput()
{
	BTNS_DDR |= SET_BTNS_ON;
	BTNS_PORT |= SET_BTNS_ON;
}

void ReadBtns()
{
	//Проверим все кнопки по циклу
	for(int i=0;i!=MAXBTN;i++)
	{
		//Если текущая кнопка == той, что нужно нажать
		if(i==ToCheckNow)
		{
			//Проверяем на нажатость
			if(bit_is_set(BTNS_PIN,CorrectButtons[ToCheckNow]))
			{
				//Ждем пока не человек отпустит кнопку
				while(bit_is_set(BTNS_PIN,CorrectButtons[ToCheckNow]));
				ToCheckNow++;
			}
		}
		else
		{
			//Реакция на не правильно нажатую кнопку
			if(bit_is_set(BTNS_PIN,CorrectButtons[ToCheckNow]))
			{
				//Ждем пока не человек отпустит кнопку
				while(bit_is_set(BTNS_PIN,CorrectButtons[ToCheckNow]));
				ToCheckNow=0;
			}
			//Реакция на не правильную кнопку
			else if(bit_is_set(BTNS_PIN,BTN6))
			{
				while(bit_is_set(BTNS_PIN,BTN6));
				ToCheckNow=0;
			}
		}
	}
}
