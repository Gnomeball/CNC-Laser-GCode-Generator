CXX = clang++
CXXFLAGS = -Wundef -Wextra -Wall -Wpedantic

MAGICK = `Magick++-config --cxxflags --cppflags --ldflags --libs`
# IMPORTS = -I/usr/include/ImageMagick-7/

SRC = src
OBJ = obj
BIN = bin

PROGRAM = parser

SOURCES = $(basename $(notdir $(shell find $(SRC) -name *.cpp)))
OBJECTS = $(addprefix $(OBJ)/, $(addsuffix .o, $(SOURCES)))
# DEPENDS := $(OBJCTS:.o=.d)

TARGET = $(BIN)/$(PROGRAM)

# Compilation

all: directories $(PROGRAM)

$(OBJ)/%.o: $(SRC)/%.cpp
	${CXX} ${CXXFLAGS} ${MAGICK}  -c $< -o $@

$(PROGRAM): $(OBJECTS)
	$(CXX) ${CXXFLAGS} ${MAGICK}  $(OBJECTS) -o $(TARGET)

# Debug

debug: CXXFLAGS += -g
debug: all

# Set up

directories:
	mkdir -p $(OBJ) $(BIN)

# Clean up

clean:
	rm -rf $(OBJ) $(BIN)

# Misc

# -include $(DEPENDS)

remake: clean all

.PHONY: clean
