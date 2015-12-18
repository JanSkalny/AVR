/*
 AppleRemote IR receiver
 =======================
 (aka UsbIrNEC)
 (based on OBJECTIVE DEVELOPMENT Software GmbH V-USB driver)
 
 @author johnny ^_^ <johnny@netvor.sk>
 @date 2010-06-24
 @license beerware

 -----------------------------------------------------------------------------
 "THE BEER-WARE LICENSE":

 <johnny@netvor.sk> wrote this file. As long as you retain this notice you
 can do whatever you want with this stuff. If we meet some day, and you think
 this stuff is worth it, you can buy me a beer (*or Kofola) in return.   

          johnny ^_^ <johnny@netvor.sk>
 -----------------------------------------------------------------------------
*/

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "usbdrv.h"

/* TSOP1738 connection */
#define IR_PIN		PINB
#define IR_PIN_BIT	PB4
#define IR_PCINT	PCINT4

/* bit off-times 
 * (prescaler/F_CPU) seconds
 * works for tiny45@16.5Mhz */
#define IR_BIT_0 15
#define IR_BIT_1 (IR_BIT_0*3)
#define IR_LEAD_MIN (IR_BIT_0*8)
#define IR_LEAD_MAX (IR_BIT_0*12)

#define IR_TIMER	TCNT0

uint16_t ir_cmd;
uint8_t ir_cmd_len=0;

volatile uint8_t ir_recv = 1;
volatile uint8_t ir_flush = 0;



static uchar    idleRate;           /* in 4 ms units */

#ifndef NULL
#define NULL    ((void *)0)
#endif

PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = { /* USB report descriptor */
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};
/* We use a simplifed keyboard report descriptor which does not support the
 * boot protocol. We don't allow setting status LEDs and we only allow one
 * simultaneous key press (except modifiers). We can therefore use short
 * 2 byte input reports.
 * The report descriptor has been created with usb.org's "HID Descriptor Tool"
 * which can be downloaded from http://www.usb.org/developers/hidpage/.
 * Redundant entries (such as LOGICAL_MINIMUM and USAGE_PAGE) have been omitted
 * for the second INPUT item.
 */

/* ------------------------------------------------------------------------- */

uchar	usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;

    if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
        if(rq->bRequest == USBRQ_HID_GET_REPORT){  /* wValue: ReportType (highbyte), ReportID (lowbyte) */
            /* we only have one report type, so don't look at wValue */
            usbMsgPtr =(void*)&ir_cmd;
            return 2;
        }else if(rq->bRequest == USBRQ_HID_GET_IDLE){
            usbMsgPtr = &idleRate;
            return 1;
        }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
            idleRate = rq->wValue.bytes[1];
        }
    }else{
        /* no vendor specific requests implemented */
    }
	return 0;
}

/* ------------------------------------------------------------------------- */
/* ------------------------ Oscillator Calibration ------------------------- */
/* ------------------------------------------------------------------------- */

/* Calibrate the RC oscillator to 8.25 MHz. The core clock of 16.5 MHz is
 * derived from the 66 MHz peripheral clock by dividing. Our timing reference
 * is the Start Of Frame signal (a single SE0 bit) available immediately after
 * a USB RESET. We first do a binary search for the OSCCAL value and then
 * optimize this value with a neighboorhod search.
 * This algorithm may also be used to calibrate the RC oscillator directly to
 * 12 MHz (no PLL involved, can therefore be used on almost ALL AVRs), but this
 * is wide outside the spec for the OSCCAL value and the required precision for
 * the 12 MHz clock! Use the RC oscillator calibrated to 12 MHz for
 * experimental purposes only!
 */
static void calibrateOscillator(void)
{
uchar       step = 128;
uchar       trialValue = 0, optimumValue;
int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* do a binary search: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* proportional to current real frequency */
        if(x < targetValue)             /* frequency still too low */
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0)
            x = -x;
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}
/*
Note: This calibration algorithm may try OSCCAL values of up to 192 even if
the optimum value is far below 192. It may therefore exceed the allowed clock
frequency of the CPU in low voltage designs!
You may replace this search algorithm with any other algorithm you like if
you have additional constraints such as a maximum CPU clock.
For version 5.x RC oscillators (those with a split range of 2x128 steps, e.g.
ATTiny25, ATTiny45, ATTiny85), it may be useful to search for the optimum in
both regions.
*/

void    usbEventResetReady(void)
{
    calibrateOscillator();
    eeprom_write_byte(0, OSCCAL);   /* store the calibrated value in EEPROM */
}

/* ------------------------------------------------------------------------- */
/* --------------------------------- main ---------------------------------- */
/* ------------------------------------------------------------------------- */


/*
 * handle PCINT from IR_PIN_BIT
 *
 * start receiving after (IR_LEAD_MIN,IR_LEAD_MAX) cycles long ,,0'' signal.
 * record next bit value if this was falling edge and we are receiving.
 */
SIGNAL (SIG_PIN_CHANGE) {
    if (ir_flush)
        return;

    // raising edge
    if (IR_PIN & _BV(IR_PIN_BIT)) {
        if (!ir_recv && IR_TIMER>IR_LEAD_MIN && IR_TIMER<IR_LEAD_MAX) 
            // start recording command
            ir_recv=1;

    // falling edge
    } else {
        if (ir_recv && ir_cmd_len<32) {
            // end of bit. record value
            if (IR_TIMER <= IR_BIT_0)
				// 0 (0~IR_BIT_0) cycles
                ir_cmd_len++;

            if (IR_TIMER > IR_BIT_0 && IR_TIMER < IR_BIT_1)
				// 1 (IR_BIT_0,IR_BIT_1) cycles
				// we're interested only in upper 16 bits (hack ^^`)
                ir_cmd |= _BV(ir_cmd_len++ - 16);
        }
    }
    IR_TIMER=0;
}

/*
 * stop receiving on IR timer overflow and mark data as complete
 */
ISR(TIMER0_OVF_vect)
{
    ir_flush = 1;
}

/**
 * IR initialisation
 */
void irInit(void) {
	// allow PCINT from port IR_PORT[IR_PORT_BIT]
	GIMSK |= _BV(PCIE);
	PCMSK |= _BV(IR_PCINT);

	// prescaler = 1/1024 + interrupt on T0
	TCCR0B |= 5;
	TIMSK |= _BV(TOIE0);
	TCNT0=0;
}

int main(void)
{
	uchar   i;
	uchar   calibrationValue;
	uint8_t blink=4;

	DDRB |= _BV(PB3);

    calibrationValue = eeprom_read_byte(0); /* calibration value from last time */
    if(calibrationValue != 0xff){
        OSCCAL = calibrationValue;
    }
	irInit();
    usbDeviceDisconnect();
    for(i=0;i<10;i++){  /* 300 ms disconnect */
        _delay_ms(15);
    }
    usbDeviceConnect();
    wdt_enable(WDTO_1S);
    usbInit();
    sei();

    for(;;){    /* main event loop */
        wdt_reset();
        usbPoll();

		// flush data notification
        if (ir_flush) {
            if (ir_cmd_len) {
				// send data from recv buffer, if any
				usbSetInterrupt((void*)&ir_cmd, 2);
				blink=8;
			}

			// clear receive buffer
            ir_cmd = 0;
            ir_cmd_len = 0;
            ir_recv = 0;
            ir_flush = 0;
        }

		if (blink > 0) {
			// blink led indicator
			_delay_ms(2);
			if (--blink > 0) 
				PORTB |= _BV(PB3);
			else 
				PORTB &= ~_BV(PB3);
		}
	}
    return 0;
}
