CC = gcc
CFLAGS = -Wall -Wno-implicit-function-declaration -g 

SDLFLAGS = -lSDL2main -lSDL2 

all: chip8_emu

chip8_emu: src/main.o src/chip8.o src/display.o src/input.o
	$(CC) -o chip8_emu src/main.o src/chip8.o src/display.o src/input.o $(SDLFLAGS)

main.o: src/main.c src/chip8.h src/display.h src/input.h
	$(CC) $(CFLAGS) -c src/main.c

chip8.o: src/chip8.h src/chip8.c
	$(CC) $(CFLAGS) -c src/chip8.c

display.o: src/display.h src/display.c
	$(CC) $(CFLAGS) -c src/display.c

input.o: src/input.h src/input.c
	$(CC) $(CFLAGS) -c src/input.c

clean:
	rm -f src/*.o

clean_all:
	rm -f src/*.o chip8_emu
