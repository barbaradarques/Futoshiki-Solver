#include <iostream>
#include "solver.hpp"

using namespace std;

int main(){
	int tests;
	cin >> tests;
	for(int i = 1; i <= tests; ++i){
		cout << i << endl;
		solve(FORWARD_CHECKING);
	}
	return 0;
}