#include "solver.hpp"

static int  size, board[MAX_SIZE][MAX_SIZE];
static char restrictions[MAX_SIZE][MAX_SIZE];
static int  nextEmptyRow, nextEmptyCol;

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
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			if(!board[i][j]){
				nextEmptyRow = i;
				nextEmptyCol = j;
				goto loopsEnd;
			}
		}
	}
	loopsEnd:
	addNumber();
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
	for(int i = 0; i < MAX_SIZE; ++i){
		for(int j = 0; j< MAX_SIZE; ++j){
			restrictions[i][j] = 0;
		}                                                   
	}
}

/*
 	Explicação detalhada da lógica por trás da matriz de restrições pode ser encontrada no relatório.
 */

void setRestriction(int row1, int col1, int row2, int col2){
	if(row1 == row2){ // se estiverem na mesma linha
		if(col1 < col2){ // se o menor está mais à esquerda 
			restrictions[row1][col1] |= LS_THAN_RIGHT;
			restrictions[row2][col2] |= GTR_THAN_LEFT;
		} else { // se o menor estiver mais à direita, inverte-se a direção do sinal
			restrictions[row1][col1] |= GTR_THAN_RIGHT;
			restrictions[row2][col2] |= LS_THAN_LEFT;
		}
	} else if(col1 == col2){ // teste redundante, pois se as linhas não são iguais, as colunas têm que ser <<
		if(row1 < row2){ // se o menor está mais acima
			restrictions[row1][col1] |= LS_THAN_BOTTOM;
			restrictions[row2][col2] |= GTR_THAN_TOP;
		} else { // se o menor está mais abaixo, inverte-se a direção do sinal
			restrictions[row1][col1] |= GTR_THAN_BOTTOM;
			restrictions[row2][col2] |= LS_THAN_TOP;
		}
	}
}

// talvez usar ponteiro pra função dependendo da flag? <<<<<<<<<<
// terminar condições <<<<<<<<<<<<<<

bool isValid(char num, char row, char col){

	// checa se já existe esse dígito naquela linha
	for(int i = 0; i < size; ++i){
		if(board[row][i] == num){
			return false;
		}
	}

	// checa se já existe esse dígito naquela coluna
	for(int i = 0; i < size; ++i){
		if(board[i][col] == num){
			return false;
		}
	}

	// Checa restrições(maior ou menor que algo):
	// Explicação: 
	// - Se AND der diferente de 0, a restrição está contida no conjunto de restrições.
	// Casos excepcionais:
	// - O dígito 1 não pode ser colocado em nenhum quadrado onde existe uma relação GTR_THAN(maior que).
	// - O dígito 4 não pode ser colocado em nenhum quadrado onde existe uma relação LS_THAN(menor que).

	// relação com o quadrado inferior
	if(restrictions[row][col] & GTR_THAN_BOTTOM){
		if(num == 1)	return false;
		if(num < board[row+1][col])	return false;
	} else if(restrictions[row][col] & LS_THAN_BOTTOM){
		if(num == 4)	return false;
		if((board[row+1][col] != 0) && (num > board[row+1][col]))	return false;
	}

	//relação com quadrado superior
	if(restrictions[row][col] & GTR_THAN_TOP){
		if(num == 1)	return false;
		if(num < board[row-1][col])	return false;
	} else if(restrictions[row][col] & LS_THAN_TOP){
		if(num == 4)	return false;
		if((board[row-1][col]!=0) && (num > board[row-1][col]))	return false;
	}

	//relação com quadrado direito
	if(restrictions[row][col] & GTR_THAN_RIGHT){
		if(num == 1)	return false;
		if(num < board[row][col+1])	return false;
	} else if(restrictions[row][col] & LS_THAN_RIGHT){
		if(num == 4)	return false;
		if((board[row][col+1]!=0) && (num > board[row][col+1]))	return false;
	}

	//relação com quadrado esquerdo
	if(restrictions[row][col] & GTR_THAN_LEFT){
		if(num == 1)	return false;
		if(num < board[row][col-1])	return false;	
	} else if(restrictions[row][col] & LS_THAN_LEFT){
		if(num == 4)	return false;
		if((board[row][col-1]!=0) && (num > board[row][col-1]))	return false;
	}

	return true;
}

/* LÓGICA NÃO CHECADA <<<<<<< */
// trocar nextEmptyROW/Col por 1 valor só com restos (poupa if) <<<<<<

bool addNumber(){
	// busca espaço vazio

	for(int n = 1; n <= 9; ++n){
		if(isValid(n, nextEmptyRow, nextEmptyRow)){
			board[nextEmptyRow][nextEmptyCol] = n;
			if(!addNumber()){ // se tiver chegado a um ponto sem solução
				// conserta nextEmpty
				// e zera posição
				board[nextEmptyRow][nextEmptyCol] = 0;
			} else { // se deu certo
				return true;
			}

		}
	}
	return false;
}