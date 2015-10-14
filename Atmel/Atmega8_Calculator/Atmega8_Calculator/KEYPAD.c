#include "KEYPAD.h"
#include <avr/io.h>
#define KEYPAD_DDR DDRB
#define KEYPAD_PORT PORTB
#define KEYPAD_PINS PINB

#define INPUT_1 PB0
#define INPUT_2 PB1
#define INPUT_3 PB2
#define INPUT_4 PB3

#define OUTPUT_1 PB4
#define OUTPUT_2 PB5
#define OUTPUT_3 PB6
#define OUTPUT_4 PB7

#define KEYPAD_INIT_ 0b11110000
uint8_t RPorts[4] = {INPUT_1,INPUT_2,INPUT_3,INPUT_4};
uint8_t OPorts[4] = {OUTPUT_1,OUTPUT_2,OUTPUT_3,OUTPUT_4};
char KEYPAD_VAL[4][4] = {{'7','8','9','/'},
						{'4','5','6','*'},
						{'1','2','3','-'},
						{'.','0','=','+'}};
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