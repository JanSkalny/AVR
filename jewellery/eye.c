#include "stdafx.h"

#include "eye.h"
#include "disp.h"
#include "rand.h"

SPRITE(eye_circle) {
	B__xxxx__
	B_xxxxxx_
	Bxxxxxxxx
	Bxxxxxxxx
	Bxxxxxxxx
	Bxxxxxxxx
	B_xxxxxx_
	B__xxxx__
};

SPRITE(eye_close1) {
	B________
	B__xxxx__
	Bxxxxxxxx
	Bxxxxxxxx
	Bxxxxxxxx
	Bxxxxxxxx
	B__xxxx__
	B________
};

SPRITE(eye_close2) {
	B________
	B__xxxx__
	B_xxxxxx_
	Bxxxxxxxx
	Bxxxxxxxx
	B_xxxxxx_
	B__xxxx__
	B________
};

SPRITE(eye_close3) {
	B________
	B________
	B________
	B_xxxxxx_
	Bxxxxxxxx
	B__xxxx__
	B________
	B________
};

SPRITE(eye_close4) {
	B________
	B________
	B________
	B__xxxx__
	B_xxxxxx_
	B__xxxx__
	B________
	B________
};

SPRITE(eye_close5) {
	B________
	B________
	B________
	B________
	B_xxxxxx_
	B________
	B________
	B________
};

uint8_t eye_duration = 0;
uint8_t eye_x=3, eye_y=3;

void draw_closing_eye(uint8_t* spr, uint8_t dly) {
	draw_sprite_e(spr);
	disp[eye_y+0] &= ~(_BV(eye_x)|_BV(eye_x+1));
	disp[eye_y+1] &= ~(_BV(eye_x)|_BV(eye_x+1));
	delay_10ms(dly);
}

void draw_open_eye(uint8_t dly) {
	draw_sprite_e(eye_circle);
	disp[eye_y+0] &= ~(_BV(eye_x)|_BV(eye_x+1));
	disp[eye_y+1] &= ~(_BV(eye_x)|_BV(eye_x+1));
	if (dly)
		delay_10ms(dly);
}

void draw_eye()
{
	uint8_t sx;

	if (eye_duration == 0) {
		switch (rnd8()%8) {
		case 0:
			eye_x = rnd8()%7;
			eye_y = 2+rnd8()%3;
			eye_duration = 2+rnd8()%5;
			break;
		case 1:
			eye_y = rnd8()%5+1;
			eye_x = 2+rnd8()%3;
			eye_duration = 2+rnd8()%5;
			break;
		case 2:
		case 3:
		case 4:
			eye_x = 3;
			eye_y = 3;
			eye_duration = 5+rnd8()%10;
			break;
		case 5:
			eye_y = 3;
			draw_open_eye(5);
			if (eye_x >= 4) {
				for (sx=eye_x; eye_x!=0; eye_x--) 
					draw_open_eye(13);
				delay_10ms(35+rnd8()%50);
				for (; eye_x!=sx; eye_x++) 
					draw_open_eye(7);
			} else {
				for (sx=eye_x; eye_x!=6; eye_x++) 
					draw_open_eye(13);
				delay_10ms(35+rnd8()%50);
				for (; eye_x!=sx; eye_x--) 
					draw_open_eye(7);
			}
			eye_duration = 2+rnd8()%10;
			break;
		case 6:
		case 7:
			draw_closing_eye(eye_close1,1);
			draw_closing_eye(eye_close2,1);
			draw_closing_eye(eye_close3,1);
			draw_closing_eye(eye_close4,1);
			draw_closing_eye(eye_close5,1);
			draw_blank();
			delay_10ms(12);
			draw_closing_eye(eye_close5,1);
			draw_closing_eye(eye_close4,1);
			draw_closing_eye(eye_close3,1);
			draw_closing_eye(eye_close2,1);
			draw_closing_eye(eye_close1,1);
		}

		draw_open_eye(0);
	} else {
		eye_duration--;
	}
}

