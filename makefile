CFLAGS = -Wall -g 
LIBS = -lallegro `pkg-config --libs $(AL_LIBS)` -lm
AL_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5
PROGRAM = ballz
SOURCES=$(wildcard sources/*.c)
OBJECTS=$(patsubst sources/%.c, obj/%.o, $(SOURCES))

all: $(PROGRAM)

play: all
	./$(PROGRAM)

$(PROGRAM): main.o $(OBJECTS)
	gcc $(CFLAGS) obj/* -o $(PROGRAM) $(LIBS)

main.o: main.c sources/engine.h sources/states.h
	@mkdir -p obj
	gcc $(CFLAGS) -c main.c -o obj/main.o $(LIBS)

$(OBJECTS): obj/%.o : sources/%.c
	@mkdir -p obj
	gcc $(CFLAGS) -c $^ $(LIBS) -o $@

tar: purge
	tar -czf ballz.tar.gz ../ballz/

clean:
	-rm -rf obj/ *~f

purge: clean
	-rm -f $(PROGRAM)
