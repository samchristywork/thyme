CC=g++
CFLAGS=-Isrc/

all: build/main

.PHONY: objects
objects: $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

build/%.o: src/%.cpp
	mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/main: build/main.o
	${CC} build/*.o ${LIBS} -o $@

build/sample: sample/sample.cpp
	${CC} sample/sample.cpp -o $@

.PHONY: run
run: build/main build/sample
	./build/main build/sample

.PHONY: clean
clean:
	rm -rf build
