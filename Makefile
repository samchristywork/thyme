all: build/main

build/main: src/main.cpp
	mkdir -p build
	g++ -o build/main src/main.cpp
