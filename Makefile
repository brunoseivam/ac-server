CC=xtensa-lx106-elf-gcc
CFLAGS=-I. -mlongcalls
LDLIBS=-nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -lhal -Wl,--end-group -lgcc
LDFLAGS=-Teagle.app.v6.ld

ac-0x00000.bin: ac
	esptool.py elf2image $^
	cp *bin host_esp

ac: ac.o ir.o led.o

ac.o: ac.c

ir.o: ir.c

led.o: led.c

flash: ac-0x00000.bin
	esptool.py --port /dev/ttyUSB0 --baud 230400 write_flash -fs 32m 0 ac-0x00000.bin 0x40000 ac-0x40000.bin

clean:
	rm -f ac ir.o led.o ac.o ac-0x00000.bin ac-0x40000.bin host_esp/ac*bin
