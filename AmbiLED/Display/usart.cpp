#include "usart.h"

#include <avr/io.h>

namespace usart {

void _putc(unsigned char c) {
	 // wait until UDR ready
	while(!(UCSRA & (1 << UDRE)));
	UDR = c;
}

void puts (char *s) {
	//	loop until *s != NULL
	while (*s) {
		_putc(*s);
		s++;
	}
}

};

