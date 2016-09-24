#include "solver.hpp"

static int  size, board[9][9];
static char restrictions[17][9]; // cols = 2*n-1 rows = n, sendo n = 9

using namespace std;

void solve(){
	int numRestrictions; // quantidade de restrições
	cin >> size;
	cin >> numRestrictions;
	resetRestrictions();
	// seta os valores iniciais
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			cin >> board[i][j];
		}
	}
	// seta as restrições
	int row1, col1, row2, col2;
	while(numRestrictions--){
		cin >> col1 >> row1 >> col2 >> row2; // entrada padrão eixo (x,y) vira matriz[y][x]
		setRestriction(row1, col1, row2, col2);
	}

	printBoard();
}

void printBoard(){
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			cout << board[i][j] << " "; // consertar pra não imprimir espaço extra no final da linha <<<<<<
		}
		cout << endl;
	}
}

void resetRestrictions(){
	for(int i = 0; i < 17; ++i){
		for(int j = 0; j< 9; ++j){
			restrictions[i][j] = 0;
		}                                                   
	}
}

/*
 	Explicação detalhada da lógica por trás da matriz de restrições pode ser encontrada no relatório.
 */

void setRestriction(int row1, int col1, int row2, int col2){
	int row, col; // linha e coluna na matriz de restrições
	int sign;
	if(row1==row2){ // se estiverem na mesma linha
		row = 2 * row1; // coloco a linha 1, mas poderia ser qualquer uma, já que são iguais <<
		if(col1 < col2){ // se o menor está mais à esquerda 
			col = col1;
			sign = LESS;
		} else { // se o menor estiver mais à direita
			col = col2;
			sign = GREATER; // inverte-se a direção do sinal
		}
	} else if(col1==col2){ // teste redundante, pois se as linhas não são iguais, as colunas têm que ser <<
		col = col1; // coloco a coluna 1, mas poderia ser qualquer uma, já que são iguais <<
		if(row1 < row2){ // se o menor está mais acima
			row = 2*row2 - 1; 
			sign = LESS;
		} else { // se o menor está mais abaixo
			row = 2*row1 -1;
			sign = GREATER; // inverte-se a direção do sinal
		}
	}
	restrictions[row][col] = sign;
}