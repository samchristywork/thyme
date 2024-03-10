CC=g++
CFLAGS=-Isrc/

all: build/thyme

.PHONY: objects
objects: $(patsubst src/%.cpp, build/%.o, $(wildcard src/*.cpp))

build/%.o: src/%.cpp
	mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/thyme: build/main.o
	${CC} build/*.o ${LIBS} -o $@

build/sample: src/sample.cpp
	${CC} src/sample.cpp -o $@

.PHONY: run
run: build/thyme build/sample
	./build/thyme build/sample

.PHONY: clean
clean:
	rm -rf build
