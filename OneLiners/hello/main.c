#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void delay_10ms(int msec)
{
	while (msec-- > 0) {
		_delay_ms(10);
	}
}
/*
	switch(r) {
	case 0: DDRA = _BV(PA0); PORTA = _BV(PA0); break;
	case 1: DDRA = _BV(PA5); PORTA = _BV(PA5); break;
	case 2: DDRB = _BV(PB0); PORTB = _BV(PB0); break;
	}

	switch(c) {
	case 0: DDRA |= _BV(PA4); break;
	case 1: DDRB |= _BV(PB5); break;
	case 2: DDRB |= _BV(PB4); break;
	}

	OCR0A = disp_row_delay[r];
}

ISR(TIMER0_COMPA_vect) {
	refresh();
	TCNT0L = 0;
}
*/
int main(void)
{
	DDRA = _BV(PA0)|_BV(PA4);

	while(1) {
		PORTA = _BV(PA4);
	}
}

