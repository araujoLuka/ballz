ALLEGRO_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5
CFLAGS = -Wall -lallegro `pkg-config --libs $(ALLEGRO_LIBS)` -lm
LIBS = ball.o block.o game.o engine.o
PROGRAM = ballz

all: $(PROGRAM)

$(PROGRAM): main.o $(LIBS)
	gcc main.o -o $(PROGRAM) $(LIBS) $(CFLAGS)

main.o: main.c ball.h block.h engine.h game.h
	gcc -c main.c -o main.o $(CFLAGS)

engine.o: engine.c engine.h ball.h game.h
	gcc -c engine.c -o engine.o $(CFLAGS)

ball.o: ball.c ball.h
	gcc -c ball.c $(CFLAGS)

block.o: block.c block.h
	gcc -c block.c $(CFLAGS)

game.o: game.c game.h
	gcc -c game.c $(CFLAGS)

clean:
	-rm -f *~f *.o

purge: clean
	-rm -f $(PROGRAM)