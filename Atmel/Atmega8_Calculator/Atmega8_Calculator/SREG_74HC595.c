#include "SREG_74HC595.h"
#include <avr/io.h>

#define SREG_DDR DDRD
#define SREG_PORT PORTD

#define ST_CP PD3
#define ST_CP_ENABLE SREG_PORT |= _BV(ST_CP)
#define ST_CP_DISABLE SREG_PORT &= ~_BV(ST_CP)

#define DS PD4
#define DS_ENABLE SREG_PORT |= _BV(DS)
#define DS_DISABLE SREG_PORT &= ~_BV(DS)

#define SH_CP PD5
#define SH_CP_ENABLE SREG_PORT |= _BV(SH_CP)
#define SH_CP_DISABLE SREG_PORT &= ~_BV(SH_CP)

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
