#pragma once

#define SNAKE_MAX_LEN 64

uint8_t snake_food;
uint8_t snake_body[SNAKE_MAX_LEN];
uint8_t snake_head;
uint8_t snake_len;

#define SNAKE_XY(x,y) (((x)<<4)|(y))
#define SNAKE_X(xy) (((xy)>>4)&0xf)
#define SNAKE_Y(xy) ((xy)&0xf)
#define SNAKE_GET_SEGMENT(i) (snake_body[(snake_head+(i))%SNAKE_MAX_LEN])
#define SNAKE_SET_SEGMENT(i,val) (snake_body[(snake_head+(i))%SNAKE_MAX_LEN]) = (val);

uint8_t snake_random_food();
void snake_start();
uint8_t snake_turn(uint8_t dx, uint8_t dy, uint8_t collisions);
void draw_snake(uint8_t frame);

