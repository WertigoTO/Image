
all: main.o winconwrapper.o maths.o
	gcc main.o winconwrapper.o maths.o -o image.exe `sdl2-config --cflags --libs`

main.o:
	gcc -c main.c

winconwrapper.o:
	gcc -c winconwrapper.c

maths.o:
	gcc -c maths.c

clean:
	rm *o image.exe