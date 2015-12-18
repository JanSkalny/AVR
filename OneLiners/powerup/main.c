/*
 PowerUP!
 --------
 push some buttons, shortly after waking up

 @author johnny ^_^ <johnny@netvor.sk>
 @date 2012-12-31
 @license beerware

 -----------------------------------------------------------------------------
 "THE BEER-WARE LICENSE":

 <johnny@netvor.sk> wrote this file. As long as you retain this notice you
 can do whatever you want with this stuff. If we meet some day, and you think
 this stuff is worth it, you can buy me a beer (*or Kofola) in return.   

          johnny ^_^ <johnny@netvor.sk>
 -----------------------------------------------------------------------------
*/

#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#ifndef NULL
#define NULL    ((void *)0)
#endif

void delay_10ms(int msec)
{
	while (msec-- > 0) {
		_delay_ms(10);
	}
}

int main(void)
{
	DDRB &= ~_BV(PB3) & ~_BV(PB4);
	PORTB &= ~_BV(PB3) & ~_BV(PB4);

	delay_10ms(200);

	// bring PB4 from Z to 0 for 300ms
	DDRB |= _BV(PB4);
	PORTB &= ~_BV(PB4);
	delay_10ms(30);	
	DDRB &= ~_BV(PB4);
	PORTB &= ~_BV(PB4);

#if 0
	// bring PB3 from Z to 0 for 300ms
	DDRB |= _BV(PB3);
	PORTB &= ~_BV(PB3);
	delay_10ms(30);	
	DDRB &= ~_BV(PB3);
	PORTB &= ~_BV(PB3);
#endif

	// power down
	set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
	power_all_disable();
	wdt_disable();

	while (1) {
		sleep_mode(); 
	}

    return 0;
}
