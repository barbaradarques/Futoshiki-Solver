#include <iostream>
#include "solver.hpp"

using namespace std;

int main(int argc, char const *argv[]){
	char method = argv[1][0]; // métodos de solução possíveis = {BACKTRACKING, FORWARD_CHECKING, MRV}
	cout << method << endl;
	ofstream file;

	switch(method){
		case BACKTRACKING:
			file.open("benchmark/backtracking.csv");
			break;
		case FORWARD_CHECKING:
			file.open("benchmark/forward_checking.csv");
			break;
		case MRV:
			file.open("benchmark/mrv.csv");
			break;
	}

	file << "# do Teste, Dimensão do Tabuleiro, Número de Atribuições, Tempo de Execução (em milissegundos)\n";
	file.flush();

	int tests;
	cin >> tests;

	
	for(int i = 1; i <= tests; ++i){
		cout << i << endl;
		file << i << ",";
		solve(method, file); 
	}

	file.close();

	return 0;
}