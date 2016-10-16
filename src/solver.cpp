#include "solver.hpp"

using namespace std;

// variáveis globais
static int  size, board[MAX_SIZE][MAX_SIZE], num_calls; 
static char restrictions[MAX_SIZE][MAX_SIZE]; // guarda as restrições de maior e menor de cada posição
static _9bits possibilities[MAX_SIZE][MAX_SIZE]; // guarda os dígitos disponíveis para cada posição
static char method; // método utilizado na solução 

// sobrecarrega o operador de comparação do _pair a fim de inverter a ordem da fila de prioridade                                      
bool operator<(const _pair & a, const _pair & b){   
    return (a.second > b.second);                                    
} 

// resolve a partida, imprime o resultado no console e salva os resultados de benchmark em arquivo
void solve(char l_method, ofstream& file){
	int numRestrictions; // quantidade de restrições
	cin >> size;
	cin >> numRestrictions;
	method = l_method;
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

	// tempo no início da execução
	auto start = chrono::steady_clock::now();

	// inicia a recursão
	addNumber();

	// tempo no final da execução
	auto end = chrono::steady_clock::now();

	// imprime o tabuleiro resolvido
	printBoard();


	// salva em arquivo a dimensão do tabuleiro, o número de atribuições, o tempo gasto na solução
	// e quanto do tabuleiro deu pra ser resolvido dentro do número máximo de atribuições possíveis 
	int filled = 0;
	for(int i = 0, digit = 0; i < size; ++i){
		for(int j = 0; j < size; ++j){
			if(board[i][j]){
				++filled;
			}
		}
	}
	
	file << size << "," << num_calls << ", " << chrono::duration_cast<chrono::duration<double, milli> >(end-start).count();
	file << "," << (double)filled/(size*size)*100 << endl;		
	file.flush();
}

// **função recursiva base do backtracking**
// preenche a próxima posição vazia no tabuleiro
bool addNumber(){
	if(num_calls == MAX_CALLS){ // se já tiver atingido o número máximo de atribuições
		return true;
	}
	++num_calls;
	int emptyRow, emptyCol; // linha e coluna do próximo espaço vazio

	// busca espaço vazio
	if(method == MRV){
		// ordena as posições em # de possibilidades crescente
		priority_queue<_pair> ordered_possibilities; // <posição no tabuleiro, # de possibilidades>
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

		// resgata a posição com menor número de possibilidades
		auto min = ordered_possibilities.top(); 
		emptyRow = min.first / size;
		emptyCol = min.first % size;

	} else {
		// encontra o primeiro espaço vazio
		for(int i = 0; i < size; ++i){
			for(int j = 0; j < size; ++j){
				if(!board[i][j]){
					emptyRow = i;
					emptyCol = j;
					goto searchEnd;		// !!! goto usado pra sair de 2 loops de uma vez só !!!
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
				// salva as possibilidades da linha e da coluna afetadas pela escolha
				_9bits savedRowPossibilities[size];
				_9bits savedColPossibilities[size];
				for(int i = 0; i < size; ++i){
					savedRowPossibilities[i] = possibilities[emptyRow][i];
					savedColPossibilities[i] = possibilities[i][emptyCol];
				}

				
				if(!updatePossibilities(n, emptyRow, emptyCol) || !addNumber()){ // se tiver detectado ou chegado a um ponto sem solução
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
	// se não encontrou nenhum valor válido para essa posição:
	return false;
}

// atualiza as possibilidades dos afetados pelo posicionamento de um novo dígito
// se não zerar as possibilidades de nenhuma outra posição, retorna true
// caso contrário, retorna false

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

		// relação com quadrado superior
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

		// relação com quadrado direito
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

		// relação com quadrado esquerdo
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

	// se não tiver zerado as possibilidades de nenhuma outra posição
	return true;
}

// verifica se um dado dígito pode ser colocado em uma certa posição
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

	
	// se não existe impedimentos para o posicionamento
	return true;
}

// adiciona tipo de restrição existente entre duas posições
void setRestriction(int row1, int col1, int row2, int col2){ // entrada: board[row1][col1] < board[row2][col2]
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

// imprime o tabuleiro
void printBoard(){
	for(int i = 0; i < size; ++i){
		for(int j = 0; j < size-1; ++j){
			cout << board[i][j] << " "; 
		}
		cout << board[i][size-1]; // o último dígito da linha não é seguido de espaço
		cout << endl;
	}
}

// zera as restrições de todas as posições
void resetRestrictions(){
	for(int i = 0; i < size; ++i){
		for(int j = 0; j< size; ++j){
			restrictions[i][j] = 0;
		}                                                   
	}
}

// seta todos os dígitos como possíveis para todas as posições
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


