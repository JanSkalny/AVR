#include "stdafx.h"

#include "io.h"
#include "disp.h"

void refresh();

/**
 * !!! nechytat !!!
 *
 * TODO do buducna
 *  - per pin nastavovat uroven podsvietenia skracovanim OCR0A
 */
void io_init()
{
	//XXX:OSCCAL = 0x76;
	//3 pre 1Mhz
	OCR0A = 3;
	TCCR0B |= _BV(CS02);//|_BV(CS00);
	TIMSK |= _BV(OCIE0A);
	TIFR |= _BV(OCF0A);

	sei();
}

#ifdef HW_861_DECODER

uint8_t active_dot = 0;
#if 0
void refresh()
{
	uint8_t row, col;

	active_dot++;
	col = active_dot % 8;
	row = (active_dot>>3) % 8;

	// vsetko na A ide do stavu vysokej impedancie (Z)
	DDRA = 0xff;
	PORTA = 0x0;

	// dekoderove vystupy su aktivne
	DDRB = _BV(PB4)|_BV(PB5)|_BV(PB6);
	PORTB = 0;

	// zobrazujeme len aktivny pixel
	if (!(disp[row] & _BV(col)))
		return;

	// aktivny riadok (katoda) pojde zo 74HC238
	switch(row) {
	//case 0 je "000" 
	case 1: PORTB |= _BV(PB4); break;
	case 2: PORTB |= _BV(PB5); break;
	case 3: PORTB |= _BV(PB5)|_BV(PB4); break;
	case 4: PORTB |= _BV(PB6); break;
	case 5: PORTB |= _BV(PB6)|_BV(PB4); break;
	case 6: PORTB |= _BV(PB6)|_BV(PB5); break;
	case 7: PORTB |= _BV(PB6)|_BV(PB5)|_BV(PB4); break;
	}

	// aktivny stlpec (anoda) zdvihneme zo Z do 0
	switch(col) {
	case 0: PORTA |= _BV(PA0); break;
	case 1: PORTA |= _BV(PA1); break;
	case 2: PORTA |= _BV(PA2); break;
	case 3: PORTA |= _BV(PA3); break;
	case 4: PORTA |= _BV(PA4); break;
	case 5: PORTA |= _BV(PA5); break;
	case 6: PORTA |= _BV(PA6); break;
	case 7: PORTA |= _BV(PA7); break;
	}
}
#endif

#endif

#ifdef HW_861_SMD

uint8_t active_dot = 0;

// @3v
//#define DEL_A 2
//#define DEL_B 13
// @5v
#define DEL_A 4
#define DEL_B 2
#define DEL_RST 40
uint8_t row_delay[8] = { DEL_A, DEL_A, DEL_B, DEL_A, DEL_RST, DEL_B, DEL_B, DEL_A };

/**
 * zobrazovatko pre pokazene attiny861
 * namiesto aktivovania spravneho pinu (0/1) sa snazime 
 * vsetko drzat v ,,Z''. (hack)
 *
 * TODO: pred finalnym palenim odstranit returny, ktore na 
 * napajkovanej doske sposobuju RESET!
 *
 * XXX: niektore pi
 */
void refresh() {
	uint8_t row, col;

	active_dot++;
	col = active_dot % 8;
	row = (active_dot>>3) % 8;


	//XXX: nastavme interval, ako dlho bude pixel svietit
	//
	// PORTB je vykonnejsi.. musime spat kratsie?
	// (mozno defektny kus)
	//
	// na B1 mame pripojeny programator (MISO),
	// potebujeme mat aspon 100ohm medzi disp. a pinom
	OCR0A = row_delay[row];


	// vsetko ide do stavu vysokej impedancie (Z)
	DDRA = 0;
	DDRB = 0;

	// zobrazujeme len aktivny pixel
	//XXX: rotate
	// row je col a col je row
	if (!(disp[col] & _BV(row)))
		return;

	PORTB = ~(_BV(PB5)|_BV(PB4)|_BV(PB2));

	// aktivny riadok (katoda) zdvihneme zo Z na 1. 
	switch(row) {
	case 0: DDRA = _BV(PA0); PORTA = _BV(PA0); break;
	case 1: DDRA = _BV(PA5); PORTA = _BV(PA5); break;
	case 2: DDRB = _BV(PB0); PORTB = _BV(PB0); break;
	case 3: DDRA = _BV(PA3); PORTA = _BV(PA3); break;
	case 4: /*XXX:*/ DDRB = _BV(PB7); PORTB = _BV(PB7); break;
	case 5: /*XXX:*/ DDRB = _BV(PB1); PORTB = _BV(PB1); break;
	case 6: DDRB = _BV(PB6); PORTB = _BV(PB6); break;
	case 7: DDRB = _BV(PB3); PORTB = _BV(PB3); break;
	}

	// aktivny stlpec (anoda) zdvihneme zo Z do 0
	switch(col) {
	case 0: DDRA |= _BV(PA4); break;
	case 1: DDRB |= _BV(PB5); break;
	case 2: DDRB |= _BV(PB4); break;
	case 3: DDRA |= _BV(PA1); break;
	case 4: DDRB |= _BV(PB2); break;
	case 5: DDRA |= _BV(PA2); break;
	case 6: DDRA |= _BV(PA6); break;
	case 7: DDRA |= _BV(PA7); break;
	}
}

#endif

ISR(TIMER0_COMPA_vect) {
	refresh();
	TCNT0L = 0;
}
