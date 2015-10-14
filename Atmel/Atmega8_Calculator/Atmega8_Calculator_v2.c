#define F_CPU 8000000
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

/*========74HC595=======*/
/**/#define SREG_DDR DDRD
/**/#define SREG_PORT PORTD
/**/
/**/#define ST_CP PD3
/**/#define ST_CP_ENABLE SREG_PORT |= _BV(ST_CP)
/**/#define ST_CP_DISABLE SREG_PORT &= ~_BV(ST_CP)
/**/
/**/#define DS PD4
/**/#define DS_ENABLE SREG_PORT |= _BV(DS)
/**/#define DS_DISABLE SREG_PORT &= ~_BV(DS)
/**/
/**/#define SH_CP PD5
/**/#define SH_CP_ENABLE SREG_PORT |= _BV(SH_CP)
/**/#define SH_CP_DISABLE SREG_PORT &= ~_BV(SH_CP)
/**/
/**/void sreg_init();
/**/void sreg_write(int val);
/*========74HC595=======*/

/*=========1602LCD============*/
/**/uint8_t LINE_L=0;
/**/#define LINE_L_MINUS LINE_L--;if(LINE_L<0){LINE_L=0;}
/**/#define LINE_L_PLUS LINE_L++;if(LINE_L>15){LCD_CLEAR;}
/**/#define LCD_DDR DDRD
/**/#define LCD_PORT PORTD
/**/
/**/#define EN_PIN PD0
/**/#define EN_ENABLE LCD_PORT |= _BV(EN_PIN)
/**/#define EN_DISABLE LCD_PORT &= ~_BV(EN_PIN)
/**/
/**/#define RW_PIN PD1
/**/#define RW_ENABLE LCD_PORT |= _BV(RW_PIN)
/**/#define RW_DISABLE LCD_PORT &= ~_BV(RW_PIN)
/**/
/**/#define RS_PIN PD2
/**/#define RS_ENABLE LCD_PORT |= _BV(RS_PIN)
/**/#define RS_DISABLE LCD_PORT &= ~_BV(RS_PIN)
/**/#define LCD_RESET_CURSOR lcd_write_cmd(0x2);//lcd_write_cmd(0b00000010);
/**/#define LCD_CLEAR lcd_write_cmd(0x1);_delay_ms(100);LINE_L=0;//lcd_write_cmd(0b00000001);
/**/#define LCD_BACKSPACE lcd_write_cmd(0x10);lcd_write_char(0x02);lcd_write_cmd(0x10);
/**/void lcd_write_cmd(int data);
/**/void lcd_init();
/**/void lcd_write_char(char ch);
/**/void lcd_write_str(char*str);
/**/void lcd_set_cursor(int line,int pos);
/*=========1602LCD============*/

/*=========МАТРИЧНАЯ КЛАВИАТУРА========*/
/**/#define KEYPAD_DDR DDRB
/**/#define KEYPAD_PORT PORTB
/**/#define KEYPAD_PINS PINB
/**/
/**/#define INPUT_1 PB0
/**/#define INPUT_2 PB1
/**/#define INPUT_3 PB2
/**/#define INPUT_4 PB3
/**/
/**/#define OUTPUT_1 PB4
/**/#define OUTPUT_2 PB5
/**/#define OUTPUT_3 PB6
/**/#define OUTPUT_4 PB7
/**/
/**/#define KEYPAD_INIT_ 0b11110000
/**/uint8_t RPorts[4] = {INPUT_1,INPUT_2,INPUT_3,INPUT_4};
/**/uint8_t OPorts[4] = {OUTPUT_1,OUTPUT_2,OUTPUT_3,OUTPUT_4};
/**/char KEYPAD_VAL[4][4] = {{'7','8','9','/'},
/**/						{'4','5','6','*'},
/**/						{'1','2','3','-'},
/**/						{'.','0','=','+'}};
/**/void keypad_init();
/**/char keypad_read();
/*=========МАТРИЧНАЯ КЛАВИАТУРА========*/

int main(void)
{
	sreg_init();//Настраиваем сдвиговый регистр
	keypad_init();//Настраиваем матричную клавиатуру
	lcd_init();//Инициализируем LCD
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
/*=========1602LCD============*/
void lcd_set_cursor(int line,int pos)
{
	pos |= 0b10000000;
	if (line == 1)
	{
		pos += 0x40;
	}
	lcd_write_cmd(pos);
}
void lcd_write_str(char*str)
{
	do
	{
		lcd_write_char(*str);
	}while(*++str);
}
void lcd_write_char(char ch)
{
	RS_ENABLE;
	sreg_write(ch);
	EN_ENABLE;
	/*_delay_ms*/_delay_us(10);
	EN_DISABLE;
	sreg_write(~ch);
	RS_DISABLE;
}
void lcd_write_cmd(int data)
{
	sreg_write(data);
	EN_ENABLE;
	/*_delay_ms*/_delay_ms(10);
	EN_DISABLE;
	sreg_write(~data);
}
void lcd_init()
{
	LCD_DDR |= (1<<EN_PIN)|(1<<RS_PIN)|(1<<RW_PIN);
	RW_DISABLE;
	RS_DISABLE;
	_delay_ms(30);
	
	lcd_write_cmd(0x30);
	/*_delay_ms*/_delay_us(20);
	lcd_write_cmd(0x30);
	/*_delay_ms*/_delay_us(20);
	lcd_write_cmd(0x30);
	/*_delay_ms*/_delay_us(20);
	
	lcd_write_cmd(0x38);//Function Set: 8-bit, 2 Line, 5?7 Dots
	/*_delay_ms*/_delay_us(20);
	
	lcd_write_cmd(0x01);//Clear Display
	/*_delay_ms*/_delay_us(20);
	
	lcd_write_cmd(0x06);//Entry Mode
	/*_delay_ms*/_delay_us(20);
	
	lcd_write_cmd(0x0F);//Display on Cursor blinking
	/*_delay_ms*/_delay_us(20);
	RS_ENABLE;
}
/*=========1602LCD============*/

/*=======74HC595========*/
void sreg_init()
{
	SREG_DDR |= (1<<ST_CP)|(1<<DS)|(1<<SH_CP);
}
void sreg_write(int val)
{
	int c,k;
	SH_CP_DISABLE;
	ST_CP_DISABLE;
	for (c = 31; c >= 0; c--)
	{
		k = val >> c;
		if (k & 1)
		{
			DS_ENABLE;
		}
		else
		{
			DS_DISABLE;
		}
		SH_CP_ENABLE;
		SH_CP_DISABLE;
	}
	ST_CP_ENABLE;
}
/*=======74HC595========*/

/*====МАТРИЧНАЯ КЛАВИАТУРА====*/
void keypad_init()
{
	KEYPAD_DDR = KEYPAD_INIT_;
	KEYPAD_PORT = 0xFF;
}
char keypad_read()
{
	for(int l=0;l<4;l++)
	{
		KEYPAD_PORT &= ~(1<<OPorts[l]);
		for(int r=0;r<4;r++)
		{
			if(bit_is_clear(KEYPAD_PINS, r)==1)
			{
				while(bit_is_clear(KEYPAD_PINS, r)==1);
				return KEYPAD_VAL[r][l];
				//lcd_write_char(KEYPAD_VAL[r][l]);
				//calculation(KEYPAD_VAL[r][l]);
			}
		}
		KEYPAD_PORT |= (1<<OPorts[l]);
	}
	return '_';
}
/*====МАТРИЧНАЯ КЛАВИАТУРА====*/


/*==========Расчеты===========*/
void calculation(char ch)
{
	LINE_L_PLUS;//Счетчик того, сколько мы ввели
	lcd_write_char(ch);//Пишем символ на экран	
	if(ch=='.')//Ввод точки
	{
		AFTER_DOT=true;
	}
	else if(ch=='+'||ch=='-'||ch=='/'||ch=='*')//Ввод операции
	{
		LCD_CLEAR;//Очистим дисплей
		OPERATION_VAL = ch;//Присвоим выбранное "дейсвтие" к переменной
		MODE=1;//Переходим к режиму заполнения числа B
		DOT_COUNTER=0.1;//Обнуляем
		AFTER_DOT=false;//Обнуляем
	}
	else if(MODE!=1 && ch == '=')
	{
		//NOTHING TO DO HERE
	}
	else if(MODE==1 && ch == '=')
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
		sprintf(RESULT_CH,"%f",CALCULATION_DATA[2]);
		lcd_write_str(RESULT_CH);
		MODE=2;			
	}
	else if(MODE==2 && ch=='.')//Очистка данных, если мы уже просчитали результат и вывели.
	{
		CLEAR_ALL_DATA;
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



/*========================Пусть пока полежит тут, может понадобится===================*/
/**///float B_VAL=0;//Переменная для хранения B
/**///#define CLEAR_ALL_DATA A_VAL=0;B_VAL=0;RESULT = 0;MODE=0;LINE_L=0;DOT_COUNTER=0.1;AFTER_DOT=false;LCD_CLEAR;//Очистка данных
/**///float A_VAL=0;//Переменная для хранения А
/**///float RESULT=0;//Полученный результат в формате float
//Калькулятор
/*
void calculation(char ch)
{
	LINE_L_PLUS;//Счетчик того, сколько мы ввели
	lcd_write_char(ch);//Пишем символ на экран
	
	switch(MODE)
	{
		case 0://Если мы вводим первый раз, то значит заполняем число А
		if(ch=='.')//Ввод точки
		{
			AFTER_DOT=true;
		}
		else if(ch=='+'||ch=='-'||ch=='/'||ch=='*')//Ввод операции
		{
			LCD_CLEAR;//Очистим дисплей
			OPERATION_VAL = ch;//Присвоим выбранное "дейсвтие" к переменной
			MODE=1;//Переходим к режиму заполнения числа B
			DOT_COUNTER=0.1;//Обнуляем
			AFTER_DOT=false;//Обнуляем
		}
		else//Два режима заполнения.
		{
			if(AFTER_DOT==false)//Заполняем что идет перед точкой.
			{
				//Пример:
				//0 = 0*10+1
				//1 = 1*10+2
				//12
				A_VAL = A_VAL*10+(ch-'0');
			}
			else//Заполяем после точки.
			{
				//12 = (12)+(1*0.1)
				//12.1 = (12)+(2*0.10)
				//12.12
				A_VAL = (A_VAL)+((ch-'0')*DOT_COUNTER);
				DOT_COUNTER=DOT_COUNTER*0.10;
			}
		}
		break;
		case 1:
		//Практически идентично тому, что мы имееем в case 0
		if(ch=='.')
		{
			AFTER_DOT=true;
		}
		else if(ch=='=')
		{
			switch(OPERATION_VAL)
			{
				case '-':
				RESULT = A_VAL-B_VAL;
				break;
				case '+':
				RESULT = A_VAL+B_VAL;
				break;
				case '/':
				RESULT = A_VAL/B_VAL;
				break;
				case '*':
				RESULT = A_VAL*B_VAL;
				break;
			}
			LCD_CLEAR;
			sprintf(RESULT_CH,"%f",RESULT);
			lcd_write_str(RESULT_CH);
			MODE=2;
		}
		else
		{
			if(AFTER_DOT==false)
			{
				B_VAL = B_VAL*10+(ch-'0');
			}
			else
			{
				B_VAL = (B_VAL)+((ch-'0')*DOT_COUNTER);
				DOT_COUNTER=DOT_COUNTER*0.10;
			}
		}
		break;
		case 2:
		if(ch=='.')
		{
			CLEAR_ALL_DATA;
		}
		break;
	}
}*/
//Расчеты, переменные
/*
#define CLEAR_ALL_DATA A_VAL=0;B_VAL=0;RESULT = 0;MODE=0;LINE_L=0;DOT_COUNTER=0.1;AFTER_DOT=false;LCD_CLEAR;//Очистка данных
float A_VAL=0;//Переменная для хранения А
uint8_t OPERATION_VAL=0;
float B_VAL=0;//Переменная для хранения B
uint8_t MODE=0;//Режим, для switch в функции calculation.

bool AFTER_DOT=false;//Мы уже вводим число после точки?
float DOT_COUNTER=0.1;//Количество 0.х
float RESULT=0;//Полученный результат в формате float
char RESULT_CH[15];//Полученный результат в формате char
void calculation(char ch);//Функция расчета
*/
/*====================================================================================*/