#include "stdafx.h"

#include "rand.h"

uint8_t EEMEM e_seed;

uint16_t rnd;

void rnd_seed() {
	uint8_t seed;

	// seed current random number generator
	// and create/store new random number for next bootup
	seed = eeprom_read_byte(&e_seed);
	rnd = ++seed;
	eeprom_write_byte(&e_seed, seed);
}

uint8_t rnd8() {
	// linearne-kongruenty generator pseudo-nahodnych cisiel :)
	rnd = (rnd * 5 + 129) % 179;
	return rnd;
}

