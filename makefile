CFLAGS = -Wall -g 
LIBS = -lallegro `pkg-config --libs $(AL_LIBS)` -lm
AL_LIBS = allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 allegro_ttf-5 allegro_audio-5 allegro_acodec-5
PROGRAM = ballz
SOURCES=$(wildcard sources/*.c)
OBJECTS=$(patsubst sources/%.c, obj/%.o, $(SOURCES))

all: check-allegro $(PROGRAM)

play: all
	./$(PROGRAM)

check-allegro:
	@# Check for allegro libraries
	@# if not found, advise to install with make install
	@pkg-config --exists $(AL_LIBS) || { echo "Allegro libraries not found. Try 'make install'"; exit 1; }
	@# Check for allegro .so files
	@ls /usr/lib/x86_64-linux-gnu/ | grep allegro 2>&1 > /dev/null || { echo "Allegro runtime not found. Try 'make install'"; exit 2; }

	@echo "Allegro check complete"
	@echo "Compiling game..."

install:
	@echo "Installing allegro libraries..."
	# add allegro ppa
	sudo add-apt-repository ppa:allegro/5.2
	# install allegro
	sudo apt install liballegro*5.2 liballegro*5-dev

$(PROGRAM): main.o $(OBJECTS)
	@echo "Linking..."
	@gcc $(CFLAGS) obj/* -o $(PROGRAM) $(LIBS)
	@echo "Done!"
	@echo "Run ./$(PROGRAM) to play"

main.o: main.c sources/engine.h sources/states.h
	@mkdir -p obj
	@echo "Compiling main file..."
	@gcc $(CFLAGS) -c main.c -o obj/main.o $(LIBS)

$(OBJECTS): obj/%.o : sources/%.c
	@mkdir -p obj
	@echo "Compiling $^..."
	@gcc $(CFLAGS) -c $^ $(LIBS) -o $@

tar: purge
	tar -czf ballz.tar.gz ../ballz/

clean:
	-rm -rf obj/ *~f

purge: clean
	-rm -f $(PROGRAM)
