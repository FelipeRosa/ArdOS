
CC = avr-gcc
CFLAGS = -Os -mmcu=atmega328p -DF_CPU=16000000UL
OBJCOPY = avr-objcopy

all: build upload

build: bin
	rm -rf obj
	mkdir obj
	$(CC) $(CFLAGS) -c main.c -o obj/main.o
	$(CC) $(CFLAGS) -c ardos/kernel/process_management.c -o obj/process_management.o
	$(CC) $(CFLAGS) -c ardos/kernel/scheduling.c -o obj/scheduling.o
	$(CC) $(CFLAGS) -c ardos/kernel/timer.c -o obj/timer.o
	
	$(CC) -mmcu=atmega328p obj/main.o obj/process_management.o obj/scheduling.o obj/timer.o -o bin/main
	$(OBJCOPY) -R .eeprom -O ihex bin/main bin/main.hex
	
	rm -rf obj
	
clean:
	rm -rf bin
	
upload:
	avrdude -F -V -p ATMEGA328P -P /dev/ttyACM0 -c arduino -b 115200 -U flash:w:bin/main.hex
	
	
bin:
	mkdir bin
