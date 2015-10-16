/*
 * Attiny2313_BLINK_ROOM.c
 *
 * Created: 16.10.2015 8:18:54
 *  Author: Vic
 */ 

#define F_CPU 8000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LEDS_DDR DDRD
#define LEDS_PORT PORTD
#define LEDS_PIN PIND

#define LED_S1 PD2
#define LED_S2 PD3
#define LED_S3 PD4

#define SET_LED_OUTPUT ~_BV(LED_S1)|~_BV(LED_S2)|~_BV(LED_S3);
#define TURN_S1_ON LEDS_PORT |=_BV(LED_S1);
#define TURN_S2_ON LEDS_PORT |=_BV(LED_S2);
#define TURN_S3_ON LEDS_PORT |=_BV(LED_S3);

#define TURN_S1_OFF LEDS_PORT &=~_BV(LED_S1);
#define TURN_S2_OFF LEDS_PORT &=~_BV(LED_S2);
#define TURN_S3_OFF LEDS_PORT &=~_BV(LED_S3);

void InitTimers();//Включение таймеров и прерываний

void HandleLeds();//Включение LED1

void InitLEDS();//Инициализация диодов

long int ms=0;

ISR(TIMER0_OVF_vect)//Прерывание по таймеру, для секунд
{
	ms++;
	HandleLeds();
	if(ms==7500){ms=0;}
}

ISR (TIMER1_OVF_vect)//Прерывание по таймеру для обнуления секунд и вывода диодов
{
	//ms++;
	//HandleLeds();
}

int main(void)
{
	InitLEDS();
	InitTimers();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

void InitTimers()
{
	//Включаем таймеры
	TIMSK = 1 << TOIE0 | 1 << TOIE1;
	TCCR1B = 1 << CS01;
	TCCR0B = 1 << CS10;
	//Разрешаем прерывания
	sei();
}

void InitLEDS()
{
	LEDS_DDR &= SET_LED_OUTPUT;
}

void HandleLeds()
{
	switch(ms)
	{
		case 500:
			TURN_S1_ON;
			TURN_S2_ON;
			TURN_S3_ON;
			
			break;
		case 1000:
			TURN_S1_OFF;
			TURN_S2_OFF;
			TURN_S3_ON;
			break;
			
		case 1500:
			TURN_S1_ON;
			TURN_S2_ON;
			TURN_S3_OFF;
			break;
		case 2000:
			TURN_S1_OFF;
			TURN_S2_OFF;
			TURN_S3_ON;
			break;
			
		case 2500:
			TURN_S1_ON;
			TURN_S2_ON;
			TURN_S3_ON;
			break;
		case 3000:
			TURN_S1_ON;
			TURN_S2_OFF;
			TURN_S3_OFF;
			break;
			
		case 3500:
			TURN_S1_OFF;
			TURN_S2_ON;
			TURN_S3_ON;
			break;
		
		case 4000:
			TURN_S1_ON;
			TURN_S2_OFF;
			TURN_S3_OFF;
			break;
		case 4500:
			TURN_S1_ON;
			TURN_S2_ON;
			TURN_S3_ON;
			break;
		case 5000:
			TURN_S1_OFF;
			TURN_S2_ON;
			TURN_S3_OFF;
			break;
		
		case 5500:
			TURN_S1_ON;
			TURN_S2_OFF;
			TURN_S3_ON;
			break;
		case 6000:
			TURN_S1_ON;
			TURN_S3_OFF;
			break;
		case 7500:
			TURN_S1_OFF;
			break;
	}
}
