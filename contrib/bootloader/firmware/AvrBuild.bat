@ECHO OFF
"C:\Program Files\Atmel\Atmel Toolchain\AVR Assembler\Native\2.1.39.1005\avrassembler\avrasm2.exe" -S labels.tmp -fI -W+ie -C V2 -o bootloader.hex -d bootloader.obj -e bootloader.eep -m bootloader.map -l bootloader.lst bootloader.asm
