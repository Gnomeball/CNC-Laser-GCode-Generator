CC = /usr/bin/g++
CCFLAGS = -Wextra -Wall -Wpedantic

CPP = /usr/bin/g++
CPPFLAGS = -Wextra -Wall -Wpedantic

MAGICK = `Magick++-config --cxxflags --cppflags --ldflags --libs`
IMPORTS = -I/usr/include/ImageMagick-7/

CPPSTD = -std=c++11

src_dir = src
obj_dir = obj
bin_dir = bin

sources = $(addprefix $(src_dir)/, $(wildcard *.cpp))

# ? there has to be a way to generate this automagically
classes = main parser grid infill line point stats
objects = $(addprefix $(obj_dir)/, $(addsuffix .o, $(classes)))

target = $(bin_dir)/parser

# Compilation

all: directories game

obj/%.o : src/%.cpp
	${CPP} $(CPPSTD) ${CPPFLAGS} ${MAGICK} ${IMPORTS} -c $< -o $@

game: $(objects)
	$(CPP) $(CPPSTD) ${CPPFLAGS} ${MAGICK} ${IMPORTS} $(objects) -o $(target)

# Debug

debug: CPPFLAGS += -ggdb
debug: all

# Set up

directories: $(obj_dir) ${bin_dir}

obj:
	mkdir -p $(obj_dir)

bin:
	mkdir -p $(bin_dir)

# Clean up

clean:
	rm -f $(objects)

# Misc

remake: all

.PHONY: clean
