#include "stdafx.h"

#include "disp.h"
#include "snake.h"
#include "rand.h"

uint8_t snake_random_food() {
	uint8_t i, food;

fail:
	food = rnd8() & 0x77;
	for (i=1; i!=snake_len; i++) {
		if (SNAKE_GET_SEGMENT(i) == food) 
			goto fail;
	}

	return food;
}

void snake_start() {
	memset(snake_body, 0, SNAKE_MAX_LEN);
	snake_head = 0;
	snake_len = 1;
	SNAKE_SET_SEGMENT(0, SNAKE_XY(4,4));
	snake_food = snake_random_food();
}

uint8_t snake_turn(uint8_t dx, uint8_t dy, uint8_t collisions) {
	uint8_t x, y, head, new_head, i, will_collide=0;

	head = SNAKE_GET_SEGMENT(0);

	x = SNAKE_X(head);
	x = (x + dx) % 8;

	y = SNAKE_Y(head);
	y = (y + dy) % 8;

	new_head = SNAKE_XY(x,y);

	// ma zmysel zistovat kolizie?
	if (collisions < 4) {
		// check if new_head is in our existing body
		// if yes, it's a collision
		for (i=0; i!=snake_len; i++) {
			if (new_head == SNAKE_GET_SEGMENT(i)) {
				will_collide = 1;
				break;
			}
		}
		if (will_collide) {
			// snake will collide, try to avoid obstacles
			// 1,0 -> 0,-1 -> -1,0, -> 0,1 -> ... (left,up,right,down,...)
			if (dx)
				return snake_turn(0, -dx, collisions+1);
			else
				return snake_turn(dy, 0, collisions+1);
		}
	}

	return new_head;
}

void draw_snake(uint8_t frame) {
	uint8_t i, head, new_head, segment, x, y, food_x, food_y, dx, dy;

	// blikame jedlom - had sa hybe len kazdy druhy frame
	disp[SNAKE_X(snake_food)] ^= _BV(SNAKE_Y(snake_food));
	if (frame%2 != 0) 
		return;

	// hlavu nasmerujeme k jedlu. najskor x, potom y.
	// najkratsim smerom, no vyhneme sa koliziam ak sa da :)
	head = SNAKE_GET_SEGMENT(0);
	x = SNAKE_X(head);
	y = SNAKE_Y(head);
	food_x = SNAKE_X(snake_food);
	food_y = SNAKE_Y(snake_food);
	if (x != food_x) {
		dx = ((x-food_x)%8 > (food_x-x)%8) ? -1 : 1; // right / left 
		dy = 0;
	} else {
		dx = 0;
		dy = ((y-food_y)%8 > (food_y-y)%8) ? -1 : 1; // up / down 
	}
	new_head = snake_turn(dx, dy, 0);
	snake_head = snake_head - 1 % SNAKE_MAX_LEN;
	SNAKE_SET_SEGMENT(0, new_head);

	// zozrali sme jedlo?
	// predlzime telo a vygenerujeme nove jedlo
	if (snake_food == SNAKE_GET_SEGMENT(0)) {
		snake_len++;
		snake_food = snake_random_food();
	}

	// zaparkovali sme hlavu do tela?
	// game over - restart
	head = SNAKE_GET_SEGMENT(0);
	for (i=1; i!=snake_len; i++) {
		if (SNAKE_GET_SEGMENT(i) == head) {
			for (i=0; i!=3; i++) {
				draw_fail();
				delay_10ms(20);
				draw_blank();
				delay_10ms(20);
			}
			snake_start();
			return;	
		}
	}

	// zobrazime snake-a
	draw_blank();
	for (i=0; i!=snake_len; i++) {
		segment = SNAKE_GET_SEGMENT(i);
		disp[SNAKE_X(segment)] |= _BV(SNAKE_Y(segment));
	}
}

