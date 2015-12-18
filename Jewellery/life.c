#include "stdafx.h"

#include "life.h"
#include "disp.h"
#include "rand.h"


uint8_t life_fist_run = 1;
uint8_t life_board[8] = {0,0,0,0,0,0,0,0};
uint8_t life_prev_board[8] = {0,0,0,0,0,0,0,0};
uint8_t life_iteration;

#define LIFE_TOGGLE(x,y) life_board[(x)] ^= _BV((y))
#define LIFE_DEAD(x,y) life_board[(x)] &= ~_BV((y))
#define LIFE_ALIVE(x,y) life_board[(x)] |= _BV((y))
#define LIFE_IS_SET(x,y) (life_prev_board[(8+x)%8]&_BV((8+y)%8)) ? 1 : 0 

void life_start()
{
	uint8_t seeds;

	memset(life_board, 0, 8);
	life_iteration = 0;
	life_fist_run = 0;

	seeds = 20+(rnd8()%30); // 20-50 seedov
	while (--seeds) 
		LIFE_TOGGLE(rnd8()%8, rnd8()%8);
}

	//memcpy(life_prev_board, life_board, 8);
	
uint8_t life_is_alive()
{
	uint8_t i;

	// najskor porovname tuto a predchadzajucu dosku
	// ak sa zhoduju, sme zaseknuty => DEAD
	if (memcmp(life_board, life_prev_board, 8) == 0)
		return 0;

	// ak zijeme prilis dlho, urcite sme umreli a zacykliki sa :)
	//  => DEAD
	if (life_iteration++ > 100)
		return 0;
		
	// nasledne sa pozrieme, ci mame niekde seedy
	// ak ano => ALIVE
	for (i=0; i!=8; i++) {
		if (life_board[i] != 0)
			return 1;
	}

	// ak sme nic nenasli => DEAD
	return 0;
}

void life_ng()
{
	uint8_t row,col,neighbours;

	// aktualna doska ide to minulosti
	memcpy(life_prev_board, life_board, 8);

	for (row=0; row!=8; row++) {
		for (col=0; col!=8; col++) {
			// spocitame susedov na starej doske
			neighbours = 0;
			neighbours += LIFE_IS_SET(row,col-1);
			neighbours += LIFE_IS_SET(row-1,col-1);
			neighbours += LIFE_IS_SET(row+1,col-1);
			neighbours += LIFE_IS_SET(row,col+1);
			neighbours += LIFE_IS_SET(row-1,col+1);
			neighbours += LIFE_IS_SET(row+1,col+1);
			neighbours += LIFE_IS_SET(row-1,col);
			neighbours += LIFE_IS_SET(row+1,col);

			// aplikujeme pravidla conwayovho zivota
			//  - ak to ma menej ako dvoch alebo viac ako troch susedov, 
			//    zomrelo to (over/under population)
			//  - ak to ma prave troch susedov, ozilo to
			if (neighbours < 2 || neighbours > 3) {
				LIFE_DEAD(row,col);
			} else if (neighbours == 3) {
				LIFE_ALIVE(row,col);
			}

		}
	}
}

void draw_life(uint8_t frame) 
{
	// znizme FPS
	if (frame % 2 != 0)
		return;

	// ak je doska mrtva, pockajme cvhilu a vygenerujme novu
	if (!life_is_alive()) {
		if (!life_fist_run)
			delay_10ms(150);
		life_start();
	} else {
		// dalsia generacia zivota :)
		life_ng();
	}

	// zobrazme aktualnu dosku
	memcpy(disp, life_board, 8);
}
