#  ATmega8:
#  Fuse Low Byte      = 0xe1 (1MHz internal - default), 0xe3 (4MHz internal), 0xe4 (8MHz internal)
#  Fuse High Byte     = 0xd9 
#
#  ATtiny25:
#
FUSE_L= 0xf8
FUSE_H= 0xd9

MCU=atmega8
F_CPU=11059200


CC=avr-gcc
CXX=avr-g++
OBJCOPY=avr-objcopy
AVRDUDE=avrdude -c usbasp -p $(MCU)

CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -DF_CPU=$(F_CPU) -Os -mcall-prologues -I. -DDEBUG_LEVEL=0
OBJS= usart.o TLC5940.o ambiled.o

##
firmware.hex: firmware.out
	$(OBJCOPY) -R .eeprom -O ihex firmware.out firmware.hex 

firmware.out: $(OBJS)
	$(CC) $(CFLAGS) -o firmware.out -Wl,-Map,firmware.map $(OBJS)

.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

##
clean:
	rm -f *.o *.map *.out *.hex

##
flash: firmware.hex
	$(AVRDUDE) -U flash:w:firmware.hex:i

##
rdfuses:
	$(AVRDUDE) -q -v
wrfuses:
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

