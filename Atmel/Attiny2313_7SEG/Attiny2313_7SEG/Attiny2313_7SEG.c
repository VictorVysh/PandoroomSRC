/*
 * Простые часы для семисегментного индикатора.
 * TODO: Оптимизация и коменты в коде
 * Program Memory Usage 	:	886 bytes   43,3 % Full
 * Data Memory Usage 		:	30 bytes   23,4 % Full
 * Автор: Вышковский Виктор
 */ 
#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//Значения для вывода "цифры" на семисегментный дисплей
//NODOT -> без точки
//DOT -> с точкой
char SEGMENT_NODOT[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
char SEGMENT_DOT[] = {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF};

//======ФУНКЦИИ======//
void setup_output();//Настройка выводных портов
void setup_input();//Настройка портов ввода(Кнопки настройки времени)
void show_number(int num,int dig);//Вывод числа на сегмент
void check_time();//Смотрим, чтобы секунды не были больше 60, минуты больше 60 и часы больше 24
void ReadInput();//Чтение ввода с кнопок

int i=0;//Нужно ли оно тут?
int ms=0;//Счёт для секунд, когда больше 1000, тогда +1 секунда
int mode=0;//Режим ввода
int time_seconds=0;//Секунды
int time_minutes=0;//Минуты
int time_hr=0;//Часы

//Для удобства
#define BTN_DDR DDRD
#define BTN_PORT PORTD
#define BTN_PIN PIND
#define BTN0 PD0
#define BTN1 PD1
#define BTN2 PD2

#define SEGMENT_DDR DDRB
#define SEGMENT_PORT PORTB
#define DIGIT_DDR DDRD
#define DIGIT_PORT PORTD

#define DIG_0 PD5
#define DIG_1 PD6
#define DIG_2 PD3
#define DIG_3 PD4

ISR(TIMER0_OVF_vect)//Прерывание по таймеру, для вывода времени
{
	ms++;
	i++;
	check_time();
	//show_number(time_minutes%10,DIG_0);
	//show_number(time_minutes/10,DIG_1);
	//show_number(time_hr%10,DIG_2);
	//show_number(time_hr/10,DIG_3);
}
ISR (TIMER1_OVF_vect)//Прерывание по таймеру для счёта времени
{
	if(ms>1000)
	{
		time_seconds++;
		ms=0;
	}
}

int main(void)
{
	//Включаем таймеры
	TIMSK = 1 << TOIE0 | 1 << TOIE1;
	TCCR1B = 1 << CS01;
	TCCR0B = 1 << CS00;
	//Разрешаем прерывания
	sei();
	setup_output();//Настраиваем семгентный дисплей
	setup_input();//Настраиваем кнопки ввода
	show_number(0,DIG_0);
    while(1)
    {
		ReadInput();//Пока у нас в фоне выводится время в главном цикле читаем состояние кнопок
    }
}
void setup_output()//Настройка вывода
{
	SEGMENT_DDR = 0xFF;//Все порты сегментов на выход
	DIGIT_DDR |= (1<<DIG_0)|(1<<DIG_1)|(1<<DIG_2)|(1<<DIG_3);//Так же все порты разрядов на выход
	DIGIT_PORT = 0xFF;//Выключаем все разряды
}
void setup_input()//Настраиваем кнопки для ввода
{
	BTN_DDR |= (0<<BTN0)|(0<<BTN1)|(0<<BTN2);
	BTN_PORT |= (1<<BTN0)|(1<<BTN1)|(1<<BTN2);
}
void show_number(int num,int dig)//Вывод числа на сегментн
{
	SEGMENT_PORT = 0xFF;
	DIGIT_PORT = (1<<BTN0)|(1<<BTN1)|(1<<BTN2);
	DIGIT_PORT &= ~(1<<dig);
	if(dig==DIG_2)
	{
		SEGMENT_PORT = SEGMENT_DOT[num];//~SEGMENT_DOT[num];
		
	}
	else
	{
		SEGMENT_PORT = SEGMENT_NODOT[num];//~SEGMENT_NODOT[num];
	}
	DIGIT_PORT |= _BV(dig);
}
void check_time()//Проверка правильности времени
{
	if(time_seconds>59)
	{
		time_seconds=0;
		time_minutes++;
	}
	else if(time_seconds<0)
	{
		time_seconds=0;
	}
	if(time_minutes>59)
	{
		time_minutes=0;
		time_hr++;
	}
	else if(time_minutes<0)
	{
		time_minutes=0;
	}
	if(time_hr>23)
	{
		time_hr=0;
		time_seconds=0;
		time_minutes=0;
	}
	else if(time_hr<0)
	{
		time_hr=0;
	}
	
}
void ReadInput()//Кнопки ввода
{
	switch(mode)//Два режима +- минуты и +- часы
	{
		case 0:
			if(bit_is_clear(BTN_PIN,BTN0)==1)//Кнопка 0 нажата
			{
				while(bit_is_clear(BTN_PIN,BTN0)==1);//Ждём, пока отпустим кнопку
				time_minutes++;//+1 минута
			}
			else if(bit_is_clear(BTN_PIN,BTN1)==1)//Кнопка 1 нажата
			{
				while(bit_is_clear(BTN_PIN,BTN1)==1);//Ждём, пока отпустим кнопку
				time_minutes--;//-1 минута
			}
			else if(bit_is_clear(BTN_PIN,BTN2)==1)//Кнопка 2 нажата
			{
				while(bit_is_clear(BTN_PIN,BTN2)==1);//Ждём, пока отпустим кнопку
				mode=1;//Переключаем режим
			}
		break;
		case 1://Идентично первому case, только +- минута будет +- час
			if(bit_is_clear(BTN_PIN,BTN0)==1)
			{
				while(bit_is_clear(BTN_PIN,BTN0)==1);
				time_hr++;
			}
			else if(bit_is_clear(BTN_PIN,BTN1)==1)
			{
				while(bit_is_clear(BTN_PIN,BTN1)==1);
				time_hr--;
			}
			else if(bit_is_clear(BTN_PIN,BTN2)==1)
			{
				while(bit_is_clear(BTN_PIN,BTN2)==1);
				mode=0;
			}
		break;
	}
}