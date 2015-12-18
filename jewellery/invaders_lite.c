#include "stdafx.h"

#include "disp.h"
#include "rand.h"
#include "invaders.h"

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



uint8_t *invaders_map[8] = {
	invader0a, invader0b,
	invader1a, invader1b,
	invader2a, invader2b,
	invader3a, invader3b
};

void draw_invaders(uint8_t frame) {
	uint8_t m = frame>>3;

	draw_sprite_e(invaders_map[2*(m>>3) + (m%2)]);
}


