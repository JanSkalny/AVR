#include "stdafx.h"

#include "disp.h"
#include "rand.h"
#include "invaders.h"

uint8_t invaders_duration = 0;
uint8_t *invaders_sprite1, *invaders_sprite2;
uint8_t invaders_prev_trans;


SPRITE(invader0a) {
	B___xx___
	B__xxxx__
	B_xxxxxx_
	Bx_xxxx_x
	Bxxxxxxxx
	B__x__x__
	B_x____x_
	Bx______x
};

SPRITE(invader0b) {
	B___xx___
	B__xxxx__
	B_xxxxxx_
	Bx_xxxx_x
	Bxxxxxxxx
	B__x__x__
	B_x_xx_x_
	B__x__x__
};

SPRITE(invader1a) {
	B_x____x_
	Bx_x__x_x
	Bxxxxxxxx
	Bxx_xx_xx
	Bxxxxxxxx
	B__xxxx__
	B__x__x__
	B_x____x_
  };

SPRITE(invader1b) {
	B_x____x_
	B__x__x__
	B_xxxxxx_
	B_x_xx_x_
	Bxxxxxxxx
	Bx_xxxx_x
	Bx_x__x_x
	B__x__x__
};

SPRITE(invader2a) {
	B________
	B__xxxx__
	Bxxxxxxxx
	Bx__xx__x
	Bxxxxxxxx
	B_xx__xx_
	Bxx_xx_xx
	B________
};

SPRITE(invader2b) {
	B__xxxx__
	Bxxxxxxxx
	Bx__xx__x
	Bxxxxxxxx
	B_xx__xx_
	Bxx_xx_xx
	B________
	B________
};

SPRITE(invader3a) {
	B__x__x__
	B___xx___
	Bx_xxxx_x
	Bxx_xx_xx
	Bxxxxxxxx
	Bx_xxxx_x
	Bx__xx__x
	Bxx____xx
};

SPRITE(invader3b) {
	B__x__x__
	B_xxxxxx_
	Bxx_xx_xx
	Bxxxxxxxx
	Bx_x__x_x
	Bx__xx__x
	Bx______x
	Bxx____xx
};


void draw_invaders(uint8_t frame) {
	uint8_t trans;

	// first frame of animation
	if (invaders_duration == 0) {
		// duration of animation
		switch (rnd8()%8) {
		case 0:
		case 1:
		case 2:
			invaders_duration = 50;
			break;
		case 3:
		case 4:
			invaders_duration = 30;
			break;
		case 6:
			invaders_duration = 20;
			break;
		case 7:
		case 5:
			invaders_duration = 0; 
			break;
		}

		// invader type
		switch(rnd8()%5) {
		case 0:
			invaders_sprite1 = invader0a;
			invaders_sprite2 = invader0b;
			break;
		case 1:
			invaders_sprite1 = invader3a;
			invaders_sprite2 = invader3b;
			break;
		case 2:
			invaders_sprite1 = invader2a;
			invaders_sprite2 = invader2b;
			break;
		case 3:
			invaders_sprite1 = invader1a;
			invaders_sprite2 = invader1b;
			break;
		}

		if (invaders_duration == 0) {
			trans = invaders_prev_trans;
			if (trans == 7) 
				invaders_prev_trans = trans = 5;
			frame_skip = 1;
		} else {
			trans = rnd8()%8;
			invaders_prev_trans = trans;
		}
		
		// transition effect
		switch(trans) {
		case 0:
		case 1:
			// shift left
			draw_shift_e(invaders_sprite1, 1);
			break;
		case 2:
		case 3:
			// shift right
			draw_shift_e(invaders_sprite1, 0);
			break;
		case 4:
		case 5:
		case 6:
			// shift scroll down
			draw_scroll_e(invaders_sprite1);
			break;
		case 7: 
			// pow
			draw_fail();
			delay_10ms(5);
			draw_blank();
			delay_10ms(7);
			draw_fail();
			delay_10ms(3);
			draw_blank();
			draw_scroll_e(invaders_sprite1);
		}
	} else {
		// inside of invader animation
		switch ((--invaders_duration&0x3)) {
		case 0: draw_sprite_e(invaders_sprite2); break;
		case 2: draw_sprite_e(invaders_sprite1); break;
		}
	}
}


