ALLEGRO_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5
CFLAGS = -Wall -lallegro `pkg-config --libs $(ALLEGRO_LIBS)` -lm
LIBS = libball.o game.o
PROGRAM = ballz

all: $(PROGRAM)

$(PROGRAM): main.o $(LIBS)
	gcc main.o -o $(PROGRAM) $(LIBS) $(CFLAGS)

main.o: main.c libball.h
	gcc -c main.c -o main.o $(CFLAGS)

libball.o: libball.c libball.h
	gcc -c libball.c $(CFLAGS)

game.o: game.c game.h
	gcc -c game.c $(CFLAGS)

clean:
	-rm -f *~f *.o

purge: clean
	-rm -f $(PROGRAM)