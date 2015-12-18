#ifndef TLC5940_H_
#define TLC5940_H_

namespace TLC5940 {
	void init(uint8_t count);
	void destroy();
	void blank();
	void display();
	void setLED(uint8_t id, uint8_t intensity);
	void fadeLED(uint8_t id, uint8_t intensity, uint16_t duration);
	void setDriverDC(uint8_t id, uint8_t value);

	void flushDC();
	void shift(uint8_t data, uint8_t len);

	void fade();

	extern bool flush;
};

#endif
