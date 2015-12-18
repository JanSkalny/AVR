#include "stdafx.h"

void delay_10ms(int msec)
{
	while (msec-- > 0) {
		_delay_ms(10);
	}
}


