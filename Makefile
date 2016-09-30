all:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11
run:
	./build/solver < futoshiki.dat
val:
	g++ src/*.cpp -I./includes -o ./build/solver -lm -std=c++11 -g -O0
	valgrind --leak-check=yes ./build/solver < futoshiki.dat
git:
	git add --all
	git commit -m "$m"
	git push