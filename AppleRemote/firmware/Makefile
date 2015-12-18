#  ATmega8:
#  Fuse Low Byte      = 0xe1 (1MHz internal - default), 0xe3 (4MHz internal), 0xe4 (8MHz internal)
#  Fuse High Byte     = 0xd9 
#
#  ATtiny25:
#
FUSE_L= 0xe1
FUSE_H= 0xdd

MCU=attiny45
F_CPU=16500000


CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude -c usbasp -p $(MCU)
AVRSIZE=avr-size

CFLAGS= -mmcu=$(MCU) -Wall -Wstrict-prototypes -DF_CPU=$(F_CPU) -Os -mcall-prologues -Iusbdrv -I. -I.. -DDEBUG_LEVEL=0
OBJS= usbdrv/usbdrv.o usbdrv/usbdrvasm.o main.o

##
all: firmware.hex

##
firmware.hex: firmware.out
	$(OBJCOPY) -R .eeprom -O ihex firmware.out firmware.hex 

firmware.out: $(OBJS)
	$(CC) $(CFLAGS) -o firmware.out -Wl,-Map,firmware.map $(OBJS)
	$(AVRSIZE) firmware.out

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.S.o:
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(CC) $(CFLAGS) -S $< -o $@


#.o: %.c $(DEPS)
#	$(CC) $(CFLAGS) -c $< -o $@

##
clean:
	rm -f *.o *.map *.out *.hex *.s usbdrv/*.o usbdrv/*.s

##
flash: firmware.hex
	$(AVRDUDE) -U flash:w:firmware.hex:i

##
rdfuses:
	$(AVRDUDE) -q -v
wrfuses:
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

