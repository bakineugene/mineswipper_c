run: build
	./build/mineswipper

build: main.c
	mkdir -p build
	gcc main.c -lSDL2 -lSDL2main -lSDL2_ttf  -o ./build/mineswipper

clean:
	rm -rf build
