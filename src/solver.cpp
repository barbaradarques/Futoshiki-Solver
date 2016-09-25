#include "solver.hpp"

static int  size, board[MAX_SIZE][MAX_SIZE], num_calls;
static char restrictions[MAX_SIZE][MAX_SIZE];
static _9bits possibilities[MAX_SIZE][MAX_SIZE];
static char method;

using namespace std;

void solve(char l_method){
	int numRestrictions; // quantidade de restrições
	cin >> size;
	cin >> numRestrictions;
	method = l_method; // <<<<<<< mudar dps pra leitura aqui dentro (ao invés de passar argumento)
	num_calls = 0; // número de chamadas recursivas/ atribuições feitas
	resetRestrictions();
	resetPossibilities();
	// seta os valores iniciais
	for(int i = 0, digit = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			cin >> digit;
			board[i][j] = digit;
			if(digit != 0)
				updatePossibilities(digit, i, j);
		}
	}
	// seta as restrições
	int row1, col1, row2, col2;
	while(numRestrictions--){
		cin >> row1 >> col1 >> row2 >> col2; 
		setRestriction(row1-1, col1-1, row2-1, col2-1); // corrige para início da contagem em 0
	}
	chrono::steady_clock::time_point start = chrono::steady_clock::now(), end;
	addNumber();
	end = chrono::steady_clock::now();
	printBoard();
	cout << num_calls << " chamadas em ";
	cout << chrono::duration_cast<chrono::duration<double> > (end-start).count() << "s." << endl;
}

void printBoard(){
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			cout << board[i][j] << " "; // consertar pra não imprimir espaço extra no final da linha <<<<<<
		}
		cout << endl;
	}
}
void printRestrictions(){ // <<<<<<<<<<<<<< apagar função
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			cout << (restrictions[i][j]==0 ? 0 : 1)  << " "; 
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
void resetPossibilities(){
	for(int i = 0; i < MAX_SIZE; ++i){
		for(int j = 0; j< MAX_SIZE; ++j){
			possibilities[i][j] = ~0; // seta todos os bits pra 1
		}                                                   
	}
}

/*
 	Explicação detalhada da lógica por trás da matriz de restrições pode ser encontrada no relatório.
 */

void setRestriction(int row1, int col1, int row2, int col2){
	// cout << row1 << col1 << " " << row2 << col2 << endl;
	if(row1 == row2){ // se estiverem na mesma linha
		if(col1 < col2){ // se o menor está mais à esquerda 
			// cout<<"|-> 1"<<endl;
			restrictions[row1][col1] |= LS_THAN_RIGHT;
			restrictions[row2][col2] |= GTR_THAN_LEFT;
		} else { // se o menor estiver mais à direita, inverte-se a direção do sinal
			// cout<<"|-> 2"<<endl;
			restrictions[row1][col1] |= GTR_THAN_RIGHT;
			restrictions[row2][col2] |= LS_THAN_LEFT;
		}
	} else if(col1 == col2){ // teste redundante, pois se as linhas não são iguais, as colunas têm que ser <<
		if(row1 < row2){ // se o menor está mais acima
			// cout<<"|-> 3"<<endl;
			restrictions[row1][col1] |= LS_THAN_BOTTOM;
			restrictions[row2][col2] |= GTR_THAN_TOP;
		} else { // se o menor está mais abaixo, inverte-se a direção do sinal
			// cout<<"|-> 4"<<endl;
			restrictions[row1][col1] |= GTR_THAN_BOTTOM;
			restrictions[row2][col2] |= LS_THAN_TOP;
		}
	}
}

// talvez usar ponteiro pra função dependendo da flag? <<<<<<<<<<

bool isValid(char num, int row, int col){
	// cout << row << " : " << col << endl;
	if(method == FORWARD_CHECKING){
		if(possibilities[row][col][num-1] == 0) // num-1 pois os dígitos começam em 1, mas o vetor em 0 
			return false;
	} else {// checa se já existe esse dígito naquela linha
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
	}

	// Checa restrições(maior ou menor que algo):
	// Explicação: 
	// - Se AND der diferente de 0, a restrição está contida no conjunto de restrições.
	

	// relação com o quadrado inferior
	if(restrictions[row][col] & GTR_THAN_BOTTOM){
		if(num < board[row+1][col])
			return false;
	} else if(restrictions[row][col] & LS_THAN_BOTTOM){
		if((board[row+1][col] != 0) && (num > board[row+1][col]))
			return false;
	}

	//relação com quadrado superior
	if(restrictions[row][col] & GTR_THAN_TOP){
		if(num < board[row-1][col])
			return false;
	} else if(restrictions[row][col] & LS_THAN_TOP){
		if((board[row-1][col] != 0) && (num > board[row-1][col]))
			return false;
	}

	//relação com quadrado direito
	if(restrictions[row][col] & GTR_THAN_RIGHT){
		if(num < board[row][col+1])
			return false;
	} else if(restrictions[row][col] & LS_THAN_RIGHT){
		if((board[row][col+1] != 0) && (num > board[row][col+1]))
			return false;
	}

	//relação com quadrado esquerdo
	if(restrictions[row][col] & GTR_THAN_LEFT){
		if(num < board[row][col-1])
			return false;	
	} else if(restrictions[row][col] & LS_THAN_LEFT){
		if((board[row][col-1] != 0) && (num > board[row][col-1]))
			return false;
	}

	return true;
}


bool addNumber(){
	++num_calls;
	//cout << "Nova chamada" << endl;
	// busca espaço vazio
	int emptyRow = 0, emptyCol = 0;
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			if(!board[i][j]){
				emptyRow = i;
				emptyCol = j;
				goto searchEnd;
			}
		}
	}
	return true;
	searchEnd:
	// testa todos os valores nessa posição
	for(int n = 1; n <= size; ++n){
		if(isValid(n, emptyRow, emptyCol)){
			board[emptyRow][emptyCol] = n;
			//===================== COM VERIFICAÇÃO ADIANTE ======================
			if(method == FORWARD_CHECKING){
				_9bits savedRowPossibilities[size];
				_9bits savedColPossibilities[size];
				for(int i = 0; i < size; ++i){
					savedRowPossibilities[i] = possibilities[emptyRow][i];
					savedColPossibilities[i] = possibilities[i][emptyCol];
				}
				
				if(!updatePossibilities(n, emptyRow, emptyCol) || !addNumber()){ // se tiver chegado a um ponto sem solução
					// desfaz a escolha
					board[emptyRow][emptyCol] = 0; 
					// recupera as possibilidades antes da escolha
					for(int i = 0; i < size; ++i){
						possibilities[emptyRow][i] = savedRowPossibilities[i];
						possibilities[i][emptyCol] = savedColPossibilities[i];
					}
				} else { // se deu certo
					return true;
				}
			//============================= BACKTRACKING APENAS ===================
			} else {
				if(!addNumber()){ // se tiver chegado a um ponto sem solução
					// desfaz a escolha
					board[emptyRow][emptyCol] = 0; 
				} else { // se deu certo
					return true;
				}
			}

		}
	}
	return false;
}

bool updatePossibilities(char num, int row, int col){
	for(int i = 0; i < size; ++i){
		if(i != col)	possibilities[row][i][num-1] = 0;
		if(i != row)	possibilities[i][col][num-1] = 0;
		if(possibilities[row][i]<<(MAX_SIZE-size) == 0 || possibilities[i][row]<<(MAX_SIZE-size) == 0){ // se acabar com as possibilidades de alguém
			return false;
		}
	}


	return true;
}
