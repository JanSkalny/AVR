#  ATmega8:
#  Fuse Low Byte      = 0xe1 (1MHz internal - default), 0xe3 (4MHz internal), 0xe4 (8MHz internal)
#  Fuse High Byte     = 0xd9 
#
#  ATtiny45:
#
FUSE_L= 0xe2
FUSE_H= 0xdf

MCU=attiny861
F_CPU=1000000

TARGET=firmware
FORMAT=ihex

CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude -c avrispv2 -p $(MCU) -P /dev/tty.PL2303-00001014
AVRSIZE=avr-size

CFLAGS= -mmcu=$(MCU) -Wall -DF_CPU=$(F_CPU) -Os 
OBJS= main.o

##
all: elf hex eep
	$(AVRSIZE) $(TARGET).elf $(TARGET).hex $(TARGET).eep

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep

.SUFFIXES: .elf .hex .eep

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

##

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) --output $@ $(LDFLAGS)

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
	rm -f *.o *.map $(TARGET).hex $(TARGET).eep $(TARGET).elf

##
flash: $(TARGET).hex $(TARGET).eep
	$(AVRDUDE) -U flash:w:$(TARGET).hex 
	#-U eeprom:w:$(TARGET).eep 

##
rdfuses:
	$(AVRDUDE) -q -v
wrfuses:
	$(AVRDUDE) -U hfuse:w:$(FUSE_H):m -U lfuse:w:$(FUSE_L):m

