run: build
	./build/tetris

build: main.c
	mkdir -p build
	gcc main.c -lSDL2 -lSDL2main -lSDL2_ttf  -o ./build/tetris

clean:
	rm -rf build
