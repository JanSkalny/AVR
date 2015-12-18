
#define F_CPU 1000000UL // 1MHz

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "temp_47k.h"

void delay_ms(unsigned int ms) {
	while(ms) {
		_delay_ms(0.96);
		ms--;
	}
}

void init(void) {
	ADMUX = _BV(REFS1) | _BV(REFS0) | _BV(ADLAR);
 	ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0);

	// enable  PD5 as output
	DDRD |= _BV(PD7);
}

int a2d(void) {
	ADCSRA |= _BV(ADSC);
	while ((ADCSRA & _BV(ADSC)) > 0);
	return ADCH;
}

int main(void) {
	int sum,i,v,t;
	unsigned char fan = 1;

	init();
	sei();

	while (1) {

		// calc avg temperature
		for (i=0,sum=0; i!=10; i++) {
			sum += a2d();
			delay_ms(20);
		}
		v=sum/10;
		t=temp_lookup[v];

		if (t > 29 && !fan) {
			// low, start fan (inverted logics)
			PORTD &= ~_BV(PD7);
			fan = 1;
		}
			
		if (t < 29 && fan) {
			// high, disable fan
			PORTD |= _BV(PD7);
			fan = 0;
		}

		delay_ms(500);	
	}
}

