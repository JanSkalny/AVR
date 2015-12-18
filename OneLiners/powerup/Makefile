#  ATmega8:
#  Fuse Low Byte      = 0xe1 (1MHz internal - default), 0xe3 (4MHz internal), 0xe4 (8MHz internal)
#  Fuse High Byte     = 0xd9 
#

FUSE_L= 0xe1
FUSE_H= 0xd9

MCU=atmega8
F_CPU=1000000


CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude -c avrispv2 -p $(MCU) -P /dev/tty.PL2303-00005014
AVRSIZE=avr-size

CFLAGS= -mmcu=$(MCU) -Wall -Wstrict-prototypes -DF_CPU=$(F_CPU) -Os -mcall-prologues -I. -I.. -DDEBUG_LEVEL=0
OBJS= main.o

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
	rm -f *.o *.map *.out *.hex *.s

##
flash: firmware.hex
	$(AVRDUDE) -U flash:w:firmware.hex:i

##
rdfuses:
	$(AVRDUDE) -q -v
wrfuses:
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

