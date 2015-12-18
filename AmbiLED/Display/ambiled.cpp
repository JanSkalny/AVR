#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h> 

#define UART_BAUD_RATE 57600
#define UART_BAUD_CALC(UART_BAUD_RATE,F_CPU) ((F_CPU)/((UART_BAUD_RATE)*16l)-1)

#include "TLC5940.h"

using namespace TLC5940;

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

	// leds
	init(3);
	blank(); // Clear framebuffer
	display(); // Display framebuffer

	// LoD
	DDRD |= _BV(PD5);
}

#define CMD__RESERVED_	0
#define CMD_INIT		1
#define CMD_TEST		2
#define CMD_RESET		3
#define CMD_FLUSH		4
#define CMD_SET_LED		5
#define CMD_FADE_LED	6
#define CMD_SPEED		7

unsigned char run_test = 1;

#define MSG_BUF_SIZE	50 

// in buffer we trust :]]
uint8_t msg_buf[MSG_BUF_SIZE];
uint8_t msg_write=0,msg_read=0;


SIGNAL (SIG_UART_RECV) { 
	// move data into buffer
	msg_buf[msg_write] = UDR;
	msg_write = (msg_write+1) % MSG_BUF_SIZE;

	if (msg_write == msg_read) 
		PORTD |= _BV(PD5); // buffer underflow, turn on teh LED of DOOM
}

void process(uint8_t data) {
	static uint8_t cmd,req=0,id=0;
	static uint16_t duration=0;

	if (!req) {
		switch(cmd = data) {
		case CMD_INIT:
			usart_putc(CMD_INIT);
			break;

		case CMD_FLUSH:
			flush = 1;
			break;

		case CMD_RESET:
			blank();
			flush = 1;
			break;

		case CMD_SET_LED:
			req = 4;
			break;

		case CMD_FADE_LED:
			req = 6;
			break;

		case CMD_TEST:
			run_test = 1;
			break;
		}
	} else {
		req--;
		switch (cmd) {
		case CMD_SET_LED:
			if (req == 3)
				id = data;	
			else 
				setLED(id+16*req,data);
			break;

		case CMD_FADE_LED:
			switch (req) {
			case 5:
				id = data;
				break;
			case 4:
				duration = data<<8;
				break;
			case 3:
				duration |= data;
				break;
			default:
				fadeLED(id+16*req,data,duration);
			}
			break;

		}
	}
}

int main(void)
{
	int v=0;

	setup();
	sei();

	// send initial character
	usart_putc(CMD_INIT);
	
	while (1) {
		// run demo
		if (run_test) {
			for (int x=0; x!=1280; x++) {
				for (unsigned char l=0; l!=16; l++) {
					v = 256 + l*16 - x;
					setLED(l,(v>0&&v<256)?v:0);
					v = 512 +100 + l*16 - x;
					setLED(l+16,(v>0&&v<256)?v:0);
					v = 768 +200 + l*16 - x;
					setLED(l+32,(v>0&&v<256)?v:0);
				}
				display();
				delay_ms(3);
			}
			blank();
			display();
			run_test = 0;
		}
	
		// display contents
		if (flush) {
			flush = 0;
			display();
		} 

		// wait
		delay_ms(1);

		// fade-(in|out) 
		fade();

		// process data from buffer (if any)
		while (msg_read != msg_write) {
			process(msg_buf[msg_read]);
			msg_read = (msg_read+1) % MSG_BUF_SIZE;
		}
	}
}

