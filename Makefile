all:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11
run:
	./build/solver < entrada.txt
val:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11 -g -O0
	valgrind --leak-check=yes ./build/solver