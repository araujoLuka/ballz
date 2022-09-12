CFLAGS = -Wall -g 
LIBS = -lallegro `pkg-config --libs $(AL_LIBS)` -lm
AL_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5
PROGRAM = ballz
SOURCES=$(wildcard sources/*.c)
OBJECTS=$(patsubst sources/%.c, bin/%.o, $(SOURCES))

all: $(PROGRAM)

play: all
	./$(PROGRAM)

$(PROGRAM): main.o $(OBJECTS)
	gcc $(CFLAGS) bin/main.o $(OBJECTS) -o $(PROGRAM) $(LIBS)

main.o: main.c sources/engine.h sources/states.h
	gcc $(CFLAGS) -c main.c -o bin/main.o $(LIBS)

$(OBJECTS): bin/%.o : sources/%.c
	gcc $(CFLAGS) -c $^ $(LIBS) -o $@

clean:
	-rm -f bin/*.o *~f

purge: clean
	-rm -f $(PROGRAM)