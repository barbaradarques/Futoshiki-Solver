#include <iostream>
#include "solver.hpp"

using namespace std;

int main(){
	int tests;
	cin >> tests;
	while(tests){
		cout << (tests--) << endl;
		solve();
	}
}