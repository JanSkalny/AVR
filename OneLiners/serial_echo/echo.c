
#define F_CPU 11059200UL	// 11.0592MHz
//#define F_CPU 1000000UL // 1MHz

#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h> 

#define UART_BAUD_RATE 9600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_CPU) ((F_CPU)/((UART_BAUD_RATE)*16l)-1)


void delay_ms(unsigned int ms) {
	while(ms) {
		_delay_ms(0.96);
		ms--;
	}
}

void usart_putc(uint8_t d) {
	 // wait until UDR ready
	while(!(UCSRA & _BV(UDRE)));
	UDR = d;
}

void setup()
{
	// set baud rate
	UBRRH = (uint8_t)(UART_BAUD_CALC(UART_BAUD_RATE,F_CPU)>>8);
	UBRRL = (uint8_t)UART_BAUD_CALC(UART_BAUD_RATE,F_CPU);
	
	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	UCSRC = (1<<URSEL)|(0<<USBS)|(3<<UCSZ0);
}

SIGNAL (SIG_UART_RECV) { 
	uint8_t data;
	data = UDR;
	usart_putc(data);
}

int main(void) {
	setup();
	sei();

	// send initial character
	usart_putc('a');

	// enable  PD5 as output
	DDRD |= (1<<PD5);
	while (1) {
		// PIN5 PORTD clear -> LED off
		PORTD &= ~(1 << PD5);
		delay_ms(500);
		// PIN5 PORTD set -> LED on
		PORTD |= (1 << PD5);
		delay_ms(500);	
	}
}

