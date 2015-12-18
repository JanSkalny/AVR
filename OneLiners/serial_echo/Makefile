# makefile, written by guido socher
MCU=atmega8
#MCU=at90s4433
CC=avr-g++
OBJCOPY=avr-objcopy
# optimize for size:
CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues
#-------------------
all: echo.hex
#-------------------
echo.hex : echo.out 
	$(OBJCOPY) -R .eeprom -O ihex echo.out echo.hex 
echo.out : echo.o 
	$(CC) $(CFLAGS) -o echo.out -Wl,-Map,echo.map echo.o 
rlc_led.o : echo.c
	avr-g++ $(CFLAGS) -Os -c echo.c
#------------------
load: echo.hex
	./prg_load_uc echo.hex
#-------------------
# fuse byte settings:
#  Atmel AVR ATmega8 
#  Fuse Low Byte      = 0xe1 (1MHz internal), 0xe3 (4MHz internal), 0xe4 (8MHz internal)
#  Fuse High Byte     = 0xd9 
#  Factory default is 0xe1 for low byte and 0xd9 for high byte
# Check this with make rdfuses
rdfuses:
	./prg_fusebit_uc -r
# use internal RC oscillator 1 Mhz
wrfuse1mhz:
	./prg_fusebit_uc -w 1
# use internal RC oscillator 4 Mhz
wrfuse4mhz:
	./prg_fusebit_uc -w 4
# use external 3-8 Mhz crystal
# Warning: you can not reset this to intenal unless you connect a crystal!!
wrfusecrystal:
	@echo "Warning: The external crystal setting can not be changed back without a working crystal"
	@echo "         You have 3 seconds to abort this with crtl-c"
	@sleep 3
	./prg_fusebit_uc -w 0
#-------------------
clean:
	rm -f *.o *.map *.out *t.hex
#-------------------
