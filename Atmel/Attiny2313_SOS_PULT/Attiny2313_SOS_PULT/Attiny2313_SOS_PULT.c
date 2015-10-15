/*
 * Attiny2313_SOS_PULT.c
 *
 * Created: 13.10.2015 14:47:17
 *  Author: Vic
 */ 

#define F_CPU 4000000
#include <avr/io.h>
#include <util/delay.h>

/*==========Порт вывода==========*/
#define LEDS_DDR DDRD
#define LEDS_PORT PORTD
#define LEDS_PIN PIND

#define LED_S1 PD2
#define LED_O1 PD3
#define LED_S2 PD4

#define SET_LED_OUTPUT ~_BV(LED_S1)|~_BV(LED_O1)|~_BV(LED_S2);
#define SET_LED_STATE_0 LEDS_PORT &= ~_BV(LED_S1);LEDS_PORT &= ~_BV(LED_O1);LEDS_PORT &= ~_BV(LED_S2);
#define SET_LED_STATE_1 LEDS_PORT |= _BV(LED_S1);
#define SET_LED_STATE_2 LEDS_PORT |= _BV(LED_S1)|_BV(LED_O1);
#define SET_LED_STATE_3 LEDS_PORT |= _BV(LED_S1)|_BV(LED_O1)|_BV(LED_S2);

/*==========Порт вывода==========*/

/*==========Кнопки==========*/
#define SOS_BUTTONS_DDR DDRB
#define SOS_BUTTONS_PORT PORTB
#define SOS_BUTTONS_PIN PINB

#define LEFT PB0
#define RIGHT PB1
#define UP PB2
#define DOWN PB3

#define SET_BTNS_ON _BV(LEFT)|_BV(RIGHT)|_BV(UP)|_BV(DOWN);
#define MAXBTNS 4

int BTNS_ARRAY[MAXBTNS] = {LEFT,RIGHT,UP,DOWN};

//Пароль, писать либо PBX либо то, что есть в define
int Password0[14] = {LEFT,DOWN,RIGHT,DOWN,LEFT,DOWN,LEFT,UP,RIGHT,LEFT,DOWN,RIGHT,DOWN,LEFT};
//int Password0[14] = {LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT,LEFT};

int CURRENT_POS_IN_PASSWORD = 0;
/*==========Кнопки==========*/

void InitButtons();
void InitLEDS();

int GetButton();
void HandlePressed(int BTN);
void HandleLED();
void SHandleLED();

int main(void)
{
	InitButtons();
	InitLEDS();
	int TMP_PRESSED=0;
    while(1)
    {
		TMP_PRESSED=GetButton();
		HandlePressed(TMP_PRESSED);			
	}
}

void InitButtons()
{
	SOS_BUTTONS_DDR = SET_BTNS_ON;
	SOS_BUTTONS_PORT = SET_BTNS_ON;
}

void InitLEDS()
{
	LEDS_DDR &= SET_LED_OUTPUT;
	SET_LED_STATE_0;
}

int GetButton()
{
	while(1)
	{
		for(int i=0;i!=MAXBTNS;i++)
		{
			if(!(SOS_BUTTONS_PIN & (1<<BTNS_ARRAY[i])) )//Получим состояние кнопки
			{
				while(!(SOS_BUTTONS_PIN & (1<<BTNS_ARRAY[i])) );
				return i;
			}
		}		
	}
}

void HandlePressed(int BTN)
{
	if(Password0[CURRENT_POS_IN_PASSWORD]==BTNS_ARRAY[BTN])
	{
		CURRENT_POS_IN_PASSWORD++;
		SHandleLED();
	}
	else
	{
		HandleLED();
	}
	for(int i=0;i!=CURRENT_POS_IN_PASSWORD;i++)
	{
		SET_LED_STATE_1;
		_delay_ms(200);
		SET_LED_STATE_0;
		_delay_ms(200);
	}
}


void HandleLED()
{
	//{LEFT,DOWN,RIGHT,DOWN,LEFT,DOWN,LEFT,UP,RIGHT,LEFT,DOWN,RIGHT,DOWN,LEFT};
		
	//{LEFT,DOWN,RIGHT,DOWN,LEFT};
	if(CURRENT_POS_IN_PASSWORD>0&&CURRENT_POS_IN_PASSWORD<=4)
	{
		CURRENT_POS_IN_PASSWORD=0;
	}
	//{DOWN,LEFT,UP,RIGHT};
	else if(CURRENT_POS_IN_PASSWORD>4&&CURRENT_POS_IN_PASSWORD<=8)
	{
		CURRENT_POS_IN_PASSWORD=5;
	}
	//{LEFT,DOWN,RIGHT,DOWN,LEFT};
	else if(CURRENT_POS_IN_PASSWORD>8&&CURRENT_POS_IN_PASSWORD<=13)
	{
		CURRENT_POS_IN_PASSWORD=9;
	}
	SHandleLED();
}

void SHandleLED()
{
	if(CURRENT_POS_IN_PASSWORD==5)
	{
		SET_LED_STATE_1;
	}
	else if(CURRENT_POS_IN_PASSWORD==9)
	{
		SET_LED_STATE_2;
	}
	else if(CURRENT_POS_IN_PASSWORD==13)
	{
		SET_LED_STATE_3;
	}
	else if(CURRENT_POS_IN_PASSWORD<4)
	{
		SET_LED_STATE_0;
	}
}