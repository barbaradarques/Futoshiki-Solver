all:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11
backtracking:
	./build/solver 1 < futoshiki.dat
forward_checking:
	./build/solver 2 < futoshiki.dat
mrv:
	./build/solver 3 < futoshiki.dat
val:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11 -g -O0
	valgrind --leak-check=yes ./build/solver 1 < futoshiki.dat
git:
	git add --all
	git commit -m "$m"
	git push