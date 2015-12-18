/**
 *   TLC5940 interface for ATmega8 avr/gcc
 *  
 * -----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE":
 *
 * <johnny@netvor.sk> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer (*or Kofola) in return.   
 *
 *      johnny ^_^ <johnny@netvor.sk>
 * -----------------------------------------------------------------------------
 */

#include <avr/interrupt.h>
#include <stdlib.h>
#include "TLC5940.h"

// port mapping
#define TLCPORT PORTB
#define TLCDDR DDRB
#define SCLK_PIN PB0
#define XLAT_PIN PB1
#define BLANK_PIN PB2
#define GSCLK_PIN PB3
#define VPRG_PIN PB4
#define SIN_PIN PB5

namespace TLC5940 {

typedef struct {
	uint8_t val;
	uint8_t dc;
	uint8_t speed;
	int16_t step;
	uint8_t count;
} OUTPUT;


static volatile bool needpulse = false;
static volatile bool transferdone = false;

bool flush = true;

// number of drivers in chain
uint8_t count;

// array with all outputs (count*16)
OUTPUT *out;


/*
 * Initialized drivers and starts interrupts.
 */
void init(uint8_t driver_count) {
	count = driver_count;

	out = (OUTPUT*)malloc(sizeof(OUTPUT)*16*count);
	blank();

	cli();

	TLCDDR |= _BV(SCLK_PIN) | _BV(XLAT_PIN) | _BV(BLANK_PIN) | _BV(GSCLK_PIN) | _BV(VPRG_PIN) | _BV(SIN_PIN);

	TLCPORT &= ~_BV(BLANK_PIN);
	TLCPORT &= ~_BV(XLAT_PIN);
	TLCPORT &= ~_BV(SCLK_PIN);
	TLCPORT &= ~_BV(GSCLK_PIN);

	flushDC(); // flush default values for dot correction (63))

	// setup pwm timer
	TCCR2 = (_BV(WGM21) | _BV(COM20));	// toggle OC2A on match -> GSCLK
	//TCCR2 = (_BV(WGM21) | _BV(COM20) | _BV(CS20));
	OCR2 = 1;
	TCNT2 = 0;

	// setup latch timer
	TCCR1A = (_BV(WGM10));	 // Fast PWM 8-bit
	TCCR1B = (_BV(CS11) | _BV(CS10) | _BV(WGM12));
	TIMSK = _BV(TOIE1);		 // Enable overflow interrupt
	TCNT1 = 0;

	sei();

	display();
}

/*
 * cleanup everything
 */
void destroy() {
	free(out);
}


/**
 * clear output frame and trasition data
 */
void blank() {
	uint8_t i;

	for (i=0; i!=count*16; i++) {
		out[i].val = 0;
		out[i].speed = 0;
		out[i].step = 0;
		out[i].count = 0;
		out[i].dc = 63;
	}
}

/*!
	Displays the current frame.
	
	NB! This function will hang if the interrupts are not running.
*/
void display() {
	uint8_t i;

	while (transferdone);
	
	TLCPORT &= ~_BV(VPRG_PIN);
	for (i=0; i<count*16; i++) {
		shift(out[i].val, 8);
		shift(0, 4);
	}
	transferdone = true;
}

/*!
	Overflow interrupt. Handles BLANK and latches on demand.
*/
ISR(TIMER1_OVF_vect)
{
	// Stop timers
	TCCR1A &= ~_BV(CS11);
	TCCR2 &= ~_BV(CS20);
	TLCPORT |= _BV(BLANK_PIN);

	// Latch only if new data is available
	if (transferdone) {
		TLCPORT |= _BV(XLAT_PIN); // latch
		TLCPORT &= ~_BV(XLAT_PIN);
		transferdone = false;
		
		// Extra SCLK pulse according to Datasheet p.18
		if (needpulse) {
			TLCPORT |= _BV(SCLK_PIN);
			TLCPORT &= ~_BV(SCLK_PIN);
			needpulse = false;
		} 
	}

	TLCPORT &= ~_BV(BLANK_PIN);

	// Restart timers
	TCNT2 = 0;
	TCNT1 = 0;
	TCCR1B |= _BV(CS11);
	TCCR2 |= _BV(CS20);
}

/*
 * shift given data to device chain
 *
 * @param data data
 * @param len number of bits to shuft
 */
void shift(uint8_t data, uint8_t len) {
	uint8_t mask;
	
	for (mask=1<<(len-1); mask; mask>>=1) {
		if (data&mask)
			TLCPORT |= _BV(SIN_PIN);
		else
			TLCPORT &= ~_BV(SIN_PIN);
		TLCPORT |= _BV(SCLK_PIN);
		TLCPORT &= ~_BV(SCLK_PIN);
	}
}


/*
 * set per-device dot correction value
 *
 * @param id device id
 * @param value dc value
 */
void setDriverDC(uint8_t id, uint8_t value) {
	uint8_t i;

	for (i=16*id; i!=16*(id+1); i++) 
		out[id].dc = value;

	flushDC();
}

void flushDC() {
	uint8_t id;

	while (transferdone);
   
	PORTB |= _BV(VPRG_PIN);
	for (id=0; id!=count*16; id++) 
		shift(out[id].dc,6);

	PORTB |= _BV(XLAT_PIN);
	PORTB &= ~_BV(XLAT_PIN);
	PORTB &= ~_BV(VPRG_PIN);

	needpulse = true;
}

void setLED(uint8_t id, uint8_t intensity) {
	out[id].speed = 0;
	out[id].val = intensity;
}

void fadeLED(uint8_t id, uint8_t intensity, uint16_t duration) {
	int16_t delta;

	delta = intensity - out[id].val;

	if (delta==0)
		return;

	if (duration < (uint16_t)abs(delta)) {
		out[id].step = abs(delta) / duration;
		// dont ask
		if (delta < 0)
			out[id].step = -out[id].step;
		out[id].count = 1 + (abs(delta) / abs(out[id].step));
		out[id].speed = 1;
	} else {
		out[id].step = (delta<0) ? -1 : 1;
		out[id].count = 1 + abs(delta);
		out[id].speed = 1 + (duration / abs(delta));
	}
}


uint16_t clki=0;

void fade() {
	uint8_t i, sim;

	clki = (clki+1) % 32768;

	// fading
	for (i=0; i!=count*16; i++) {
		if (out[i].speed > 0 && (clki%out[i].speed==0)) {
			sim = out[i].val + out[i].step;
			if (out[i].step > 0 && sim < out[i].val) {
				out[i].count = 0;
				sim = 255;
			} else if (out[i].step < 0 && sim > out[i].val) {
				out[i].count = 0;
				sim = 0;
			} else { 
				out[i].count--;
			}

			out[i].val = sim;
			flush = 1;

			if (out[i].count == 0)
				out[i].speed = 0;
		}
	}
}

}

