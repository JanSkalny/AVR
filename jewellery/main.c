/*

 @author johnny ^_^ <johnny@netvor.sk>
 @date 2013-12-17
 @license beerware

 -----------------------------------------------------------------------------
 "THE BEER-WARE LICENSE":

 <johnny@netvor.sk> wrote this file. As long as you retain this notice you
 can do whatever you want with this stuff. If we meet some day, and you think
 this stuff is worth it, you can buy me a beer (*or Kofola) in return.   

          johnny ^_^ <johnny@netvor.sk>
 -----------------------------------------------------------------------------
*/

#include "stdafx.h"

#include "disp.h"
#include "io.h"
#include "rand.h"
#include "fire.h"
#include "snake.h"
#include "invaders.h"
#include "eye.h"
#include "heart.h"
#include "life.h"

#define MODE_EYE 0
#define MODE_LIFE 1
#define MODE_SNAKE 2
#define MODE_FIRE 3
#define MODE_SPACE_INVADERS 4
#define MODE_HEART 5
#define NUM_MODES 6

uint8_t EEMEM e_mode = 0;
uint8_t mode;

uint8_t frame_skip=0;

int main(void)
{
	uint8_t new_mode=0, frame=0;

	// inicializacia displayu 
	io_init();

	// zistime, co mame zobrazovat (z eepromy)
	mode = (eeprom_read_byte(&e_mode) + 1) % NUM_MODES;

	// niektore animacie potrebuju extra inicializaciu, napr. generator nahodnych cisiel
	switch (mode) {
	case MODE_SNAKE:
		rnd_seed();
		snake_start();
		break;
	case MODE_SPACE_INVADERS:
	case MODE_EYE:
		rnd_seed();
		break;
	}
	
	// 
	while(1) { 
		// ak zobrazujeme aspon 10 frameov novy rezim, ulozime si ho do eepromy
		if (!new_mode && frame > 10) {
			new_mode = 1;
			eeprom_write_byte(&e_mode, mode);
		}

		switch (mode) {
		case MODE_SNAKE:
			draw_snake(frame);
			break;
		case MODE_FIRE:
			draw_fire(frame);
			break;
		case MODE_SPACE_INVADERS:
			draw_invaders(frame);
			break;
		case MODE_EYE:
			draw_eye();
			break;
		case MODE_HEART:
			draw_heart(frame);
			break;
		case MODE_LIFE:
			draw_life(frame);
			break;
		}
	
		// niektore animacie nemaju rady, ak spime (frame_skip=1)
		frame++;
		if (frame_skip) {
			frame_skip = 0;
		} else {
			delay_10ms(10);
		}
	}

	return 0;
}


