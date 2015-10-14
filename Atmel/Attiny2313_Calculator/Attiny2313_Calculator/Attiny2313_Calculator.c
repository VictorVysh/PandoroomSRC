#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

/*=======Расчеты======*/
/**/int A_VAL=0;
/**/uint8_t OPERATION_VAL=0;
/**/int B_VAL=0;
/**/uint8_t MODE=0;
/**/int RESULT=0;
/**/char RESULT_CH[15];
/**/void calculation(char ch);
/**/int soft_div(int a,int b);
/**/int soft_multp(int a,int b);
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
/**/int LINE_L=0;
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
/**/#define LCD_CLEAR lcd_write_cmd(0x1);//lcd_write_cmd(0b00000001);
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
/**/						{'?','0','=','+'}};
/**/void keypad_init();
/**/void keypad_read();
/*=========МАТРИЧНАЯ КЛАВИАТУРА========*/

int main(void)
{
	sreg_init();//Настраиваем сдвиговый регистр
	keypad_init();//Настраиваем матричную клавиатуру
	lcd_init();//Инициализируем LCD
	//lcd_write_str("Attiny2313");
	//lcd_set_cursor(1,0);
	//lcd_write_str("Calculator");
    while(1)
    {
		keypad_read();//Считываем нажатия
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
	/*_delay_ms*/_delay_us(10);
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
void keypad_read()
{
	for(int l=0;l<4;l++)
	{
		KEYPAD_PORT &= ~(1<<OPorts[l]);
		for(int r=0;r<4;r++)
		{
			if(bit_is_clear(KEYPAD_PINS, r)==1)
			{
				while(bit_is_clear(KEYPAD_PINS, r)==1);
				//lcd_write_char(KEYPAD_VAL[r][l]);
				calculation(KEYPAD_VAL[r][l]);
			}
		}
		KEYPAD_PORT |= (1<<OPorts[l]);
	}
}
/*====МАТРИЧНАЯ КЛАВИАТУРА====*/

/*==========Расчеты===========*/
int soft_multp(int a,int b)
{
	int TEMP_VAL = a;
	a = 0;
	for (int i = 0; i < b; i++)
	{
		a = TEMP_VAL + a;
	}
	return a;
}
int soft_div(int a,int b)
{
	int TEMP_VAL = 0;
	while (a>=b)
	{
		a = a - b;
		TEMP_VAL++;
	}
	return TEMP_VAL;
}
void calculation(char ch)
{
	switch(MODE)
	{
		case 0:
			if(ch=='?')
			{
				LINE_L_MINUS;
				A_VAL = soft_div(A_VAL,10);//A_VAL/10;
				LCD_BACKSPACE;
			}
			else if(ch=='+'||ch=='-'||ch=='/'||ch=='*')
			{
				LINE_L_PLUS;
				OPERATION_VAL = ch;
				lcd_write_char(ch);
				MODE=1;
			}
			else
			{
				LINE_L_PLUS;
				A_VAL = soft_multp(A_VAL,10)+(ch-'0');//A_VAL*10+(ch-'0');
				lcd_write_char(ch);
			}
		break;
		case 1:
			if(ch=='?')
			{
				LINE_L_MINUS;
				B_VAL = soft_div(B_VAL,10);//B_VAL/10;
				LCD_BACKSPACE;
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
					RESULT = soft_div(A_VAL,B_VAL);//A_VAL/B_VAL;
					break;
					case '*':
					RESULT = soft_multp(A_VAL,B_VAL);//A_VAL*B_VAL;
					break;
				}
				LINE_L_PLUS;
				lcd_write_char(ch);
				itoa(RESULT,RESULT_CH,10);
				lcd_write_str(RESULT_CH);
				MODE=2;
			}
			else
			{
				LINE_L_PLUS;
				B_VAL = soft_multp(B_VAL,10)+(ch-'0');//B_VAL*10+(ch-'0');
				lcd_write_char(ch);
			}
		break;
		case 2:
			if(ch=='?')
			{
				A_VAL=0;
				B_VAL=0;
				RESULT = 0;
				MODE=0;
				LINE_L=0;
				LCD_CLEAR;
			}
		break;		
	}
}
/*==========Расчеты===========*/