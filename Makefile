all:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11
run:
	./build/solver < entrada.txt
val:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11 -g -O0
	valgrind --leak-check=yes ./build/solver < entrada.txt
git:
	git add --all
	git commit -m "$m"
	git push