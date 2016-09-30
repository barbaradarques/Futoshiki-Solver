#include "solver.hpp"

using namespace std;

static int  size, board[MAX_SIZE][MAX_SIZE], num_calls;
static char restrictions[MAX_SIZE][MAX_SIZE];
static _9bits possibilities[MAX_SIZE][MAX_SIZE];
static char method;

// sobrecarrega o operador de comparação do _pair                                        
bool operator<(const _pair & a, const _pair & b){   
    return (a.second > b.second);                                    
} 


void solve(char l_method){
	int numRestrictions; // quantidade de restrições
	cin >> size;
	cin >> numRestrictions;
	method = l_method; // <<<<<<< mudar dps pra leitura aqui dentro (ao invés de passar argumento)
	num_calls = 0; // número de chamadas recursivas/atribuições feitas
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
	if(num_calls<MAX_CALLS){
		cout << num_calls << " chamadas em ";
	} else {
		int zeros = 0;
		for(int i = 0, digit = 0; i < size; ++i){
			for(int j = 0; j < size; ++j){
				if(!board[i][j]){
					++zeros;
				}
			}
		}
		cout << "A execução excedeu o número máximo de atribuições(10^6)."<< endl;

		cout << (double)zeros/(size*size)*100 <<"\% do tabuleiro foi completado em "; // <<<<<<<<<<<<<< mudar nome de 'tabuleiro'
	}
	cout << chrono::duration_cast<chrono::duration<double> > (end-start).count() << "s." << endl;
}

void printBoard(){
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size-1; ++j){
			cout << board[i][j] << " "; 
		}
		cout << board[i][size-1]; // o último dígito da linha não é seguido de espaço
		cout << endl;
	}
}

void resetRestrictions(){
	for(int i = 0; i < size; ++i){
		for(int j = 0; j< size; ++j){
			restrictions[i][j] = 0;
		}                                                   
	}
}
void resetPossibilities(){
	_9bits initial = ~0; // status inicial padrão, marca todos os dígitos válidos com 1 e inválidos com 0
	for(int i = MAX_SIZE-1; i > size-1; --i){
		initial[i] = 0;
	}

	// seta as possibilidades de todos os quadrados com o status inicial
	for(int i = 0; i < size; ++i){
		for(int j = 0; j< size; ++j){
			possibilities[i][j] = initial;
		}                                                   
	}
}

void setRestriction(int row1, int col1, int row2, int col2){
	if(row1 == row2){ // se estiverem na mesma linha
		if(col1 < col2){ // se o menor está mais à esquerda 
			restrictions[row1][col1] |= LS_THAN_RIGHT;
			restrictions[row2][col2] |= GTR_THAN_LEFT;
		} else { // se o menor estiver mais à direita, inverte-se a direção do sinal
			restrictions[row1][col1] |= LS_THAN_LEFT;
			restrictions[row2][col2] |= GTR_THAN_RIGHT;
		}
	} else { // se estiverem na mesma coluna 
		if(row1 < row2){ // se o menor está mais acima
			restrictions[row1][col1] |= LS_THAN_BOTTOM;
			restrictions[row2][col2] |= GTR_THAN_TOP;
		} else { // (row2 < row1) se o menor está mais abaixo, inverte-se a direção do sinal
			restrictions[row1][col1] |= LS_THAN_TOP;
			restrictions[row2][col2] |= GTR_THAN_BOTTOM;
		}
	}
}


bool isValid(char num, int row, int col){
	
	if(method != BACKTRACKING){
		// ===== Checagem baseada na verificação adiante ======

		if(possibilities[row][col][num-1] == 0) // num-1 pois os dígitos começam em 1, mas o vetor em 0 
			return false;

	} else {
		// ================ Checagem manual ===================

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
	}

	

	return true;
}


bool addNumber(){
	if(num_calls == MAX_CALLS){ // se já tiver atingido o número máximo de atribuições
		return true;
	}
	++num_calls;
	int emptyRow, emptyCol;
	// busca espaço vazio
	if(method == MRV){
		priority_queue<_pair> ordered_possibilities; 
		for(int i = 0; i < size; ++i){
			for(int j = 0; j < size; ++j){
				if(!board[i][j]){
					ordered_possibilities.push(make_pair((char)(i*size + j), possibilities[i][j].count()));
				}
			}
		}

		if(ordered_possibilities.empty()){
			return true; // se não encontrar nenhum espaço vazio é porque deu tudo certo
		}

		auto min = ordered_possibilities.top();
		emptyRow = min.first / size;
		emptyCol = min.first % size;

	} else {
		for(int i = 0; i < size; ++i){
			for(int j = 0; j < size; ++j){
				if(!board[i][j]){
					emptyRow = i;
					emptyCol = j;
					goto searchEnd;
				}
			}
		}
		return true; // se não encontrar nenhum espaço vazio é porque deu tudo certo
	}
	searchEnd:
	// testa todos os valores nessa posição
	for(int n = 1; n <= size; ++n){
		if(isValid(n, emptyRow, emptyCol)){
			board[emptyRow][emptyCol] = n;
			//============================= BACKTRACKING APENAS ===================
			if(method == BACKTRACKING){
				if(!addNumber()){ // se tiver chegado a um ponto sem solução
					// desfaz a escolha
					board[emptyRow][emptyCol] = 0; 
				} else { // se deu certo
					return true;
				}
			//===================== COM VERIFICAÇÃO ADIANTE(COM OU SEM MVR) ======================
			} else {
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
			} 

		}
	}
	return false;
}

bool updatePossibilities(char num, int row, int col){
	// atualiza quadrados da mesma linha ou mesma coluna
	for(int i = 0; i < size; ++i){
		// se não for o quadrado selecionado, retira a possibilidade do dígito em questão
		if(i != col)	possibilities[row][i][num-1] = 0;
		if(i != row)	possibilities[i][col][num-1] = 0;
		// testa se as modificações acabam com as possibilidades de alguém
		if(possibilities[row][i] == 0 || possibilities[i][row] == 0){
			return false;
		}
	}

	// atualiza os valores daqueles que tem uma relação de desigualdade com ele
	if(restrictions[row][col]){
		// relação com o quadrado inferior
		if(restrictions[row][col] & GTR_THAN_BOTTOM){
			for(int i = size; i > num; --i){
				possibilities[row+1][col][i-1] = 0;
			}
			if(possibilities[row+1][col] == 0)
				return false;
		} else if(restrictions[row][col] & LS_THAN_BOTTOM){
			for(int i = num; i > 0; --i){
				possibilities[row+1][col][i-1] = 0;
			}
			if(possibilities[row+1][col] == 0)
				return false;
		}

		//relação com quadrado superior
		if(restrictions[row][col] & GTR_THAN_TOP){
			for(int i = size; i > num; --i){
				possibilities[row-1][col][i-1] = 0;
			}
			if(possibilities[row-1][col] == 0)
				return false;
		} else if(restrictions[row][col] & LS_THAN_TOP){
			for(int i = num; i > 0; --i){
				possibilities[row-1][col][i-1] = 0;
			}
			if(possibilities[row-1][col] == 0)
				return false;
		}

		//relação com quadrado direito
		if(restrictions[row][col] & GTR_THAN_RIGHT){
			for(int i = size; i > num; --i){
				possibilities[row][col+1][i-1] = 0;
			}
			if(possibilities[row][col+1] == 0)
				return false;
		} else if(restrictions[row][col] & LS_THAN_RIGHT){
			for(int i = num; i > 0; --i){
				possibilities[row][col+1][i-1] = 0;
			}
			if(possibilities[row][col+1] == 0)
				return false;
		}

		//relação com quadrado esquerdo
		if(restrictions[row][col] & GTR_THAN_LEFT){
			for(int i = size; i > num; --i){
				possibilities[row][col-1][i-1] = 0;
			}
			if(possibilities[row][col-1] == 0)
				return false;	
		} else if(restrictions[row][col] & LS_THAN_LEFT){
			for(int i = num; i > 0; --i){
				possibilities[row][col-1][i-1] = 0;
			}
			if(possibilities[row][col-1] == 0)
				return false;
		}
	}

	return true;
}
