#include "SREG_74HC595.h"
#include "LCD1602.h"
#include <avr/io.h>
#include <util/delay.h>

#define LCD_DDR DDRD
#define LCD_PORT PORTD

#define EN_PIN PD0
#define EN_ENABLE LCD_PORT |= _BV(EN_PIN)
#define EN_DISABLE LCD_PORT &= ~_BV(EN_PIN)

#define RW_PIN PD1
#define RW_ENABLE LCD_PORT |= _BV(RW_PIN)
#define RW_DISABLE LCD_PORT &= ~_BV(RW_PIN)

#define RS_PIN PD2
#define RS_ENABLE LCD_PORT |= _BV(RS_PIN)
#define RS_DISABLE LCD_PORT &= ~_BV(RS_PIN)

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
	sreg_init();
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