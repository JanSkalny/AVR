#!/bin/sh

AVRDUDE="avrdude -c avrispv2 -p attiny861 -P /dev/tty.PL2303-00001014"

#$AVRDUDE -q -v
#$AVRDUDE -e
#XXX:8MHz $AVRDUDE -U lfuse:w:0xe2:m -U hfuse:w:0xde:m -U efuse:w:0x00:m 
#$AVRDUDE -U lfuse:w:0x62:m -U hfuse:w:0xde:m -U efuse:w:0x00:m 
$AVRDUDE -U flash:w:bootloader.hex
