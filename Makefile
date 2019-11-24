CC = gcc -g -Wall

all:
	$(CC) main.c cube.c -o lesson05 -lGL -lGLU `sdl2-config --cflags --libs` -lm

clean:
	@echo Cleaning up...
	@rm lesson05
	@echo Done.
