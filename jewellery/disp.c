#include "stdafx.h"

#include "disp.h"

uint8_t disp[8];

void draw_fail() {
	uint8_t i;
	for (i=0; i!=8; i++)
		disp[i] = 0xff;
	//memset(disp, 0xff, 8);
}

void draw_blank() {
	//uint8_t i;
	//for (i=0; i!=8; i++)
	//	disp[i] = 0x0;
	memset(disp, 0, 8);
}

#ifndef DISP_LITE

void draw_shift_e(uint8_t *spr, uint8_t dir)
{
	uint8_t buf[8], i, r;

#ifdef USE_EEPROM
	eeprom_read_block(buf, spr, 8);
#else
	memcpy(buf, spr, 8);
#endif

	for (i=0; i!=12; i++) {
		delay_10ms(5);
		for (r=0; r!=8; r++) {
			if (dir) {
				disp[r] >>= 1;
				if (i > 4) 
					disp[r] |= (buf[r] << (11-i));
			} else {
				disp[r] <<= 1;
				if (i > 4) 
					disp[r] |= (buf[r] >> (11-i));
			}
			
		}
	}
}

void draw_scroll_e(uint8_t *spr)
{
	uint8_t buf[8], i, r;

#ifdef USE_EEPROM
	eeprom_read_block(buf, spr, 8);
#else
	memcpy(buf, spr, 8);
#endif
	
	for (i=0; i!=12; i++) {
		delay_10ms(5);
		for (r=7; r>0; r--) 
			disp[r] = disp[r-1];
		if (i > 3)
			disp[0] = buf[11-i];
		else
			disp[0] = 0;
	}
}

void draw_sprite_e(uint8_t *spr)
{
#ifdef USE_EEPROM
	eeprom_read_block(disp, spr, 8);
#else
	memcpy(disp, spr, 8);
#endif
}

#endif 
