#include "stdafx.h"

#include "disp.h"
#include "fire.h"

void draw_fire(uint8_t frame) {
	draw_blank();

	uint8_t i, m;

#ifdef FIRE_LITE
	// ohen v kazdom stlpci
	for (i=0; i!=8; i++) {
		m = (0+frame+8*i) % 19;	if (m<=8) disp[7-m] |= _BV(i);
		m = (1+frame+4*i) % 17;	if (m<=6) disp[7-m] |= _BV(i);
		m = (1+frame+2*i) % 11;	if (m<=4) disp[7-m] |= _BV(i);
		m = (9+frame-4*i) % 7;	if (m<=3) disp[7-m] |= _BV(i);
		m = (7+frame-8*i) % 4;	if (m<=3) disp[7-m] |= _BV(i);
		m = (3+frame+i) % 3;	if (m<=2) disp[7-m] |= _BV(i);
		m = (frame+i) % 2;	if (m<=0) disp[7-m] |= _BV(i);
	}
#else
	// ohen v kazdom stlpci
	for (i=0; i!=8; i++) {
		m = (1*frame+0+i) % 3;	if (m<=6)  disp[8-m] |= _BV(i);
		m = (2*frame+1+i) % 3;	if (m<=6)  disp[8-m] |= _BV(i);
		m = (3*frame+0+i) % 5;	if (m<=3)  disp[8-m] |= _BV(i);
		m = (1*frame+2+i) % 8;	if (m<=6)  disp[8-m] |= _BV(i);
		m = (1*frame+1+i) % 11;	if (m<=4)  disp[8-m] |= _BV(i);
		m = (2*frame+0+i) % 14;	if (m<=4)  disp[8-m] |= _BV(i);
	}

	// stredne iskry
	m = (frame+2)%8;	if (m<=4) disp[8-m] |= _BV(0);
	m = (frame+4)%11;	if (m<=6) disp[8-m] |= _BV(1);
	m = (frame+1)%16;	if (m<=5) disp[8-m] |= _BV(2);
	m = (frame+3)%10;	if (m<=7) disp[8-m] |= _BV(3);
	m = (frame+6)%13;	if (m<=5) disp[8-m] |= _BV(4);
	m = (frame+1)%14;	if (m<=6) disp[8-m] |= _BV(5);
	m = (frame+2)%15;	if (m<=5) disp[8-m] |= _BV(6);
	m = (frame+3)%8;	if (m<=4) disp[8-m] |= _BV(7);
	
	// horne iskry
	m = (frame+1)%9;	if (m<=8) disp[8-m] |= _BV(0);
	m = (frame+3)%12;	if (m<=8) disp[8-m] |= _BV(1);
	m = (frame+0)%17;	if (m<=8) disp[8-m] |= _BV(2);
	m = (frame+2)%11;	if (m<=8) disp[8-m] |= _BV(3);
	m = (frame+5)%14;	if (m<=8) disp[8-m] |= _BV(4);
	m = (frame+0)%15;	if (m<=8) disp[8-m] |= _BV(5);
	m = (frame+1)%16;	if (m<=8) disp[8-m] |= _BV(6);
	m = (frame+2)%9;	if (m<=8) disp[8-m] |= _BV(7);
#endif
}

