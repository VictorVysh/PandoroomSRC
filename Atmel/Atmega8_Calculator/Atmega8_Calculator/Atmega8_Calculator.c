/*
Настройка фьюзов:
	LOW:E4
	Без внешнего кварца.
	
Используемые детали:
	LCD1602 - дисплей
	Клавиатура матричная 4х4
	74HC595 - сдвиговый регистр
	Atmega8 - камень
*/

#define F_CPU 8000000

#include "KEYPAD.h"
#include "LCD1602.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>

/*=======Расчеты======*/
/**/#define CLEAR_ALL_DATA for(int i=0;i<3;i++){CALCULATION_DATA[i]=0;};MODE=0;LINE_L=0;DOT_COUNTER=0.1;AFTER_DOT=false;LCD_CLEAR;//Очистка данных
/**/float CALCULATION_DATA[3] = {0.0,0.0,0.0};//Чтобы не плодить разных переменных, решил всё в один массив запихать.
/**/uint8_t OPERATION_VAL=0;//Действие */+-
/**/uint8_t MODE=0;//Режим, для switch в функции calculation.
/**/bool AFTER_DOT=false;//Мы уже вводим число после точки?
/**/float DOT_COUNTER=0.1;//Количество 0.х
/**/char RESULT_CH[15];//Полученный результат в формате char
/**/void calculation(char ch);//Функция расчета
/*=======Расчеты======*/

/*=========1602LCD============*/
uint8_t LINE_L=0;
#define LCD_GOTO_SECOND_LINE lcd_write_cmd(0b11000000);

#define PRINT_INPUT_A lcd_write_str("[A]");LCD_GOTO_SECOND_LINE;
#define PRINT_INPUT_B lcd_write_str("[B]");LCD_GOTO_SECOND_LINE;
#define PRINT_INPUT_DOT lcd_write_str("[INPUT DOT]");LCD_GOTO_SECOND_LINE;

#define LCD_RESET_CURSOR lcd_write_cmd(0x2);
#define LCD_CLEAR lcd_write_cmd(0x1);_delay_ms(100);LINE_L=0;
#define LCD_BACKSPACE lcd_write_cmd(0x10);lcd_write_char(0x02);lcd_write_cmd(0x10);
#define LINE_L_MINUS LINE_L--;if(LINE_L<0){LINE_L=0;}
#define LINE_L_PLUS LINE_L++;if(LINE_L>15){LCD_CLEAR;}
/*=========1602LCD============*/

int main(void)
{
	lcd_init();//Инициализируем LCD и Настраиваем сдвиговый регистр
	keypad_init();//Настраиваем матричную клавиатуру
	PRINT_INPUT_A;
	char TempKeypadVar;//Временная переменная для хранения введенного числа с матричной клавиатуры.
	while(1)
	{
		TempKeypadVar = keypad_read();//Считываем нажатия
		if(TempKeypadVar!='_')//Если keypad_read возвратил нам всё кроме символа "_"
		{
			calculation(TempKeypadVar);//Перходим к счету.
		}
	}
}
/*==========Расчеты===========*/
void calculation(char ch)
{
	LINE_L_PLUS;//Счетчик того, сколько мы ввели
	lcd_write_char(ch);//Пишем символ на экран
	if(MODE!=2 && ch=='.')
	{
		AFTER_DOT=true;
	}
	else if(MODE==2 && ch=='.')
	{
		CLEAR_ALL_DATA;
		PRINT_INPUT_A;
	}
	else if(ch=='+'||ch=='-'||ch=='/'||ch=='*')//Ввод операции
	{
		LCD_CLEAR;//Очистим дисплей
		OPERATION_VAL = ch;//Присвоим выбранное "дейсвтие" к переменной
		MODE=1;//Переходим к режиму заполнения числа B
		DOT_COUNTER=0.1;//Обнуляем
		AFTER_DOT=false;//Обнуляем
		PRINT_INPUT_B;
	}
	else if(MODE>0 && ch == '=')
	{
		switch(OPERATION_VAL)
		{
		case '-':
			CALCULATION_DATA[2] = CALCULATION_DATA[0]-CALCULATION_DATA[1];
			break;
		case '+':
			CALCULATION_DATA[2] = CALCULATION_DATA[0]+CALCULATION_DATA[1];
			break;
		case '/':
			CALCULATION_DATA[2] = CALCULATION_DATA[0]/CALCULATION_DATA[1];
			break;
		case '*':
			CALCULATION_DATA[2] = CALCULATION_DATA[0]*CALCULATION_DATA[1];
			break;
		}
		LCD_CLEAR;
		PRINT_INPUT_DOT;
		sprintf(RESULT_CH,"%f",CALCULATION_DATA[2]);
		lcd_write_str(RESULT_CH);
		MODE=2;			
	}
	else//Два режима.
	{
		if(AFTER_DOT==false)//Заполняем что идет перед точкой.
		{
			CALCULATION_DATA[MODE] = CALCULATION_DATA[MODE]*10+(ch-'0');
		}
		else//Заполяем после точки.
		{
			CALCULATION_DATA[MODE] = (CALCULATION_DATA[MODE])+((ch-'0')*DOT_COUNTER);
			DOT_COUNTER=DOT_COUNTER*0.10;
		}
	}
}
/*==========Расчеты===========*/