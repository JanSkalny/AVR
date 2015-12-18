#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/cpufunc.h>  

#ifndef NULL
#define NULL    ((void *)0)
#endif

void delay_10ms(int msec);

//extern uint8_t frame;
extern uint8_t frame_skip;

