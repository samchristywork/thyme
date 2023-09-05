CC=g++

all: build/main

.PHONY: objects
objects: $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

build/%.o: src/%.cpp
	mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/main: build/main.o
	${CC} build/*.o ${LIBS} -o $@
