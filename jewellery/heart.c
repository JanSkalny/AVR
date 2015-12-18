#include "stdafx.h"

#include "heart.h"
#include "disp.h"

SPRITE(heart1) {
	B_xx__xx_
	Bxxxxxxxx
	Bxxxxxxxx
	Bxxxxxxxx
	B_xxxxxx_
	B__xxxx__
	B___xx___
	B________
};

SPRITE(heart2) {
	B________
	B__x__x__
	B_xxxxxx_
	B_xxxxxx_
	B__xxxx__
	B___xx___
	B________
	B________
};

void draw_heart(uint8_t frame) {
	switch (frame%8) {
	case 0: draw_sprite_e(heart2); break;
	case 2: draw_sprite_e(heart1); break;
	}
}

