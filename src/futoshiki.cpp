#include "futoshiki.hpp"

using namespace std;



// sobrecarrega o operador de comparação do _pair a fim de inverter a ordem da fila de prioridade                                      
struct Compare {
    bool operator()(_pair& a, _pair& b) {
        return a.second > b.second;
    }
};


Futoshiki::Futoshiki(char method, ofstream& file)
	: method_(method), file_(file){}


void Futoshiki::init(){
	int numRestrictions; // quantidade de restrições
	cin >> size_;
	cin >> numRestrictions;

	num_calls_ = 0; // número de chamadas recursivas/atribuições feitas
	resetRestrictions();
	resetPossibilities();
	// seta os valores iniciais
	for(int i = 0, digit = 0; i < size_; ++i){
		for(int j = 0; j < size_; ++j){
			cin >> digit;
			board_[i][j] = digit;
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
}

// resolve a partida, imprime o resultado no console e salva os resultados de benchmark em arquivo
void Futoshiki::solve(){
	// tempo no início da execução
	auto start = chrono::steady_clock::now();

	// inicia a recursão
	switch(method_){
		case BACKTRACKING:
			fillNextBCK();
			break;
		case FORWARD_CHECKING:
			fillNextFC();
			break;
		case MRV:
			fillNextMRV();
			break;
	}

	// tempo no final da execução
	auto end = chrono::steady_clock::now();

	// imprime o tabuleiro resolvido
	printBoard();

	// salva em arquivo a dimensão do tabuleiro, o número de atribuições, o tempo gasto na solução
	// e quanto do tabuleiro deu pra ser resolvido dentro do número máximo de atribuições possíveis 
	int filled = 0;
	for(int i = 0, digit = 0; i < size_; ++i){
		for(int j = 0; j < size_; ++j){
			if(board_[i][j]){
				++filled;
			}
		}
	}
	
	file_ << size_ << "," << num_calls_ << ", " << chrono::duration_cast<chrono::duration<double, milli> >(end-start).count();
	file_ << "," << (double)filled/(size_*size_)*100 << endl;		
	file_.flush();
}

bool Futoshiki::fillNextBCK(){
	if(num_calls_ == MAX_CALLS){ // se já tiver atingido o número máximo de atribuições
		return true;
	}
	++num_calls_;

	int emptyRow, emptyCol; // linha e coluna do próximo espaço vazio
	
	// encontra o primeiro espaço vazio
	for(int i = 0; i < size_; ++i){
		for(int j = 0; j < size_; ++j){
			if(!board_[i][j]){
				emptyRow = i;
				emptyCol = j;
				goto searchEnd;		// !!! goto usado pra sair de 2 loops de uma vez só !!!
			}
		}
	}
	return true; // se não encontrar nenhum espaço vazio é porque deu tudo certo
	
	searchEnd:
	// testa todos os valores nessa posição
	for(int n = 1; n <= size_; ++n){
		if(isValid(n, emptyRow, emptyCol)){
			board_[emptyRow][emptyCol] = n;
			if(!fillNextBCK()){ // se tiver chegado a um ponto sem solução
				// desfaz a escolha
				board_[emptyRow][emptyCol] = 0; 
			} else { // se deu certo
				return true;
			}
		}
	}
	// se não encontrou nenhum valor válido para essa posição:
	return false;
}

bool Futoshiki::fillNextFC(){
	if(num_calls_ == MAX_CALLS){ // se já tiver atingido o número máximo de atribuições
		return true;
	}
	++num_calls_;
	
	int emptyRow, emptyCol; // linha e coluna do próximo espaço vazio

	// encontra o primeiro espaço vazio
	for(int i = 0; i < size_; ++i){
		for(int j = 0; j < size_; ++j){
			if(!board_[i][j]){
				emptyRow = i;
				emptyCol = j;
				goto searchEnd;		// !!! goto usado pra sair de 2 loops de uma vez só !!!
			}
		}
	}
	return true; // se não encontrar nenhum espaço vazio é porque deu tudo certo
	
	searchEnd:
	// testa todos os valores nessa posição
	for(int n = 1; n <= size_; ++n){
		if(isValid(n, emptyRow, emptyCol)){
			board_[emptyRow][emptyCol] = n;
			
			// salva as possibilidades da linha e da coluna afetadas pela escolha
			_9bits savedRowPossibilities[size_];
			_9bits savedColPossibilities[size_];

			for(int i = 0; i < size_; ++i){
				savedRowPossibilities[i] = possibilities_[emptyRow][i];
				savedColPossibilities[i] = possibilities_[i][emptyCol];
			}

			
			if(!updatePossibilities(n, emptyRow, emptyCol) || !fillNextFC()){ // se tiver detectado ou chegado a um ponto sem solução
				// desfaz a escolha
				board_[emptyRow][emptyCol] = 0; 
				// recupera as possibilidades antes da escolha
				for(int i = 0; i < size_; ++i){
					possibilities_[emptyRow][i] = savedRowPossibilities[i];
					possibilities_[i][emptyCol] = savedColPossibilities[i];
				}
			} else { // se deu certo
				return true;
			}
		}
	}
	// se não encontrou nenhum valor válido para essa posição:
	return false;
}


// **função recursiva base do backtracking**
// preenche a próxima posição vazia no tabuleiro
bool Futoshiki::fillNextMRV(){
	if(num_calls_ == MAX_CALLS){ // se já tiver atingido o número máximo de atribuições
		return true;
	}
	++num_calls_;
	int emptyRow, emptyCol; // linha e coluna do próximo espaço vazio

	// busca espaço vazio

	// ordena as posições em # de possibilidades crescente
	priority_queue<_pair, vector<_pair>, Compare> ordered_possibilities; // <posição no tabuleiro, # de possibilidades>
	for(int i = 0; i < size_; ++i){
		for(int j = 0; j < size_; ++j){
			if(!board_[i][j]){
				ordered_possibilities.push(make_pair((char)(i*size_ + j), possibilities_[i][j].count()));
			}
		}
	}

	if(ordered_possibilities.empty()){
		return true; // se não encontrar nenhum espaço vazio é porque deu tudo certo
	}

	// resgata a posição com menor número de possibilidades
	auto min = ordered_possibilities.top(); 
	emptyRow = min.first / size_;
	emptyCol = min.first % size_;
	
	// testa todos os valores nessa posição
	for(int n = 1; n <= size_; ++n){
		if(isValid(n, emptyRow, emptyCol)){
			board_[emptyRow][emptyCol] = n;
			
			// salva as possibilidades da linha e da coluna afetadas pela escolha
			_9bits savedRowPossibilities[size_];
			_9bits savedColPossibilities[size_];

			for(int i = 0; i < size_; ++i){
				savedRowPossibilities[i] = possibilities_[emptyRow][i];
				savedColPossibilities[i] = possibilities_[i][emptyCol];
			}

			if(!updatePossibilities(n, emptyRow, emptyCol) || !fillNextMRV()){ // se tiver detectado ou chegado a um ponto sem solução
				// desfaz a escolha
				board_[emptyRow][emptyCol] = 0; 
				// recupera as possibilidades antes da escolha
				for(int i = 0; i < size_; ++i){
					possibilities_[emptyRow][i] = savedRowPossibilities[i];
					possibilities_[i][emptyCol] = savedColPossibilities[i];
				}
			} else { // se deu certo
				return true;
			}
		}
	}
	// se não encontrou nenhum valor válido para essa posição:
	return false;
}

// atualiza as possibilidades dos afetados pelo posicionamento de um novo dígito
// se não zerar as possibilidades de nenhuma outra posição, retorna true
// caso contrário, retorna false

bool Futoshiki::updatePossibilities(char num, int row, int col){
	// atualiza quadrados da mesma linha ou mesma coluna
	for(int i = 0; i < size_; ++i){
		// se não for o quadrado selecionado, retira a possibilidade do dígito em questão
		if(i != col)	possibilities_[row][i][num-1] = 0;
		if(i != row)	possibilities_[i][col][num-1] = 0;
		// testa se as modificações acabam com as possibilidades de alguém
		if(possibilities_[row][i] == 0 || possibilities_[i][row] == 0){
			return false;
		}
	}

	// atualiza os valores daqueles que tem uma relação de desigualdade com ele
	if(restrictions_[row][col]){
		// relação com o quadrado inferior
		if(restrictions_[row][col] & GTR_THAN_BOTTOM){
			for(int i = size_; i > num; --i){
				possibilities_[row+1][col][i-1] = 0;
			}
			if(possibilities_[row+1][col] == 0)
				return false;
		} else if(restrictions_[row][col] & LS_THAN_BOTTOM){
			for(int i = num; i > 0; --i){
				possibilities_[row+1][col][i-1] = 0;
			}
			if(possibilities_[row+1][col] == 0)
				return false;
		}

		// relação com quadrado superior
		if(restrictions_[row][col] & GTR_THAN_TOP){
			for(int i = size_; i > num; --i){
				possibilities_[row-1][col][i-1] = 0;
			}
			if(possibilities_[row-1][col] == 0)
				return false;
		} else if(restrictions_[row][col] & LS_THAN_TOP){
			for(int i = num; i > 0; --i){
				possibilities_[row-1][col][i-1] = 0;
			}
			if(possibilities_[row-1][col] == 0)
				return false;
		}

		// relação com quadrado direito
		if(restrictions_[row][col] & GTR_THAN_RIGHT){
			for(int i = size_; i > num; --i){
				possibilities_[row][col+1][i-1] = 0;
			}
			if(possibilities_[row][col+1] == 0)
				return false;
		} else if(restrictions_[row][col] & LS_THAN_RIGHT){
			for(int i = num; i > 0; --i){
				possibilities_[row][col+1][i-1] = 0;
			}
			if(possibilities_[row][col+1] == 0)
				return false;
		}

		// relação com quadrado esquerdo
		if(restrictions_[row][col] & GTR_THAN_LEFT){
			for(int i = size_; i > num; --i){
				possibilities_[row][col-1][i-1] = 0;
			}
			if(possibilities_[row][col-1] == 0)
				return false;	
		} else if(restrictions_[row][col] & LS_THAN_LEFT){
			for(int i = num; i > 0; --i){
				possibilities_[row][col-1][i-1] = 0;
			}
			if(possibilities_[row][col-1] == 0)
				return false;
		}
	}

	// se não tiver zerado as possibilidades de nenhuma outra posição
	return true;
}

// verifica se um dado dígito pode ser colocado em uma certa posição
bool Futoshiki::isValid(char num, int row, int col){
	if(method_ != BACKTRACKING){
		// ===== Checagem baseada na verificação adiante ======

		if(possibilities_[row][col][num-1] == 0) // num-1 pois os dígitos começam em 1, mas o vetor em 0 
			return false;

	} else {
		// ================ Checagem manual ===================

		// checa se já existe esse dígito naquela linha
		for(int i = 0; i < size_; ++i){
			if(board_[row][i] == num){
				return false;
			}
		}

		// checa se já existe esse dígito naquela coluna
		for(int i = 0; i < size_; ++i){
			if(board_[i][col] == num){
				return false;
			}
		}

		// Checa restrições(maior ou menor que algo):
		// Explicação: 
		// - Se AND der diferente de 0, a restrição está contida no conjunto de restrições.
		

		// relação com o quadrado inferior
		if(restrictions_[row][col] & GTR_THAN_BOTTOM){
			if(num < board_[row+1][col])
				return false;
		} else if(restrictions_[row][col] & LS_THAN_BOTTOM){
			if((board_[row+1][col] != 0) && (num > board_[row+1][col]))
				return false;
		}

		//relação com quadrado superior
		if(restrictions_[row][col] & GTR_THAN_TOP){
			if(num < board_[row-1][col])
				return false;
		} else if(restrictions_[row][col] & LS_THAN_TOP){
			if((board_[row-1][col] != 0) && (num > board_[row-1][col]))
				return false;
		}

		//relação com quadrado direito
		if(restrictions_[row][col] & GTR_THAN_RIGHT){
			if(num < board_[row][col+1])
				return false;
		} else if(restrictions_[row][col] & LS_THAN_RIGHT){
			if((board_[row][col+1] != 0) && (num > board_[row][col+1]))
				return false;
		}

		//relação com quadrado esquerdo
		if(restrictions_[row][col] & GTR_THAN_LEFT){
			if(num < board_[row][col-1])
				return false;	
		} else if(restrictions_[row][col] & LS_THAN_LEFT){
			if((board_[row][col-1] != 0) && (num > board_[row][col-1]))
				return false;
		}
	}

	
	// se não existe impedimentos para o posicionamento
	return true;
}

// adiciona tipo de restrição existente entre duas posições
void Futoshiki::setRestriction(int row1, int col1, int row2, int col2){ // entrada: board_[row1][col1] < board_[row2][col2]
	if(row1 == row2){ // se estiverem na mesma linha
		if(col1 < col2){ // se o menor está mais à esquerda 
			restrictions_[row1][col1] |= LS_THAN_RIGHT;
			restrictions_[row2][col2] |= GTR_THAN_LEFT;
		} else { // se o menor estiver mais à direita, inverte-se a direção do sinal
			restrictions_[row1][col1] |= LS_THAN_LEFT;
			restrictions_[row2][col2] |= GTR_THAN_RIGHT;
		}
	} else { // se estiverem na mesma coluna 
		if(row1 < row2){ // se o menor está mais acima
			restrictions_[row1][col1] |= LS_THAN_BOTTOM;
			restrictions_[row2][col2] |= GTR_THAN_TOP;
		} else { // (row2 < row1) se o menor está mais abaixo, inverte-se a direção do sinal
			restrictions_[row1][col1] |= LS_THAN_TOP;
			restrictions_[row2][col2] |= GTR_THAN_BOTTOM;
		}
	}
}

// imprime o tabuleiro
void Futoshiki::printBoard(){
	for(int i = 0; i < size_; ++i){
		for(int j = 0; j < size_-1; ++j){
			cout << board_[i][j] << " "; 
		}
		cout << board_[i][size_-1]; // o último dígito da linha não é seguido de espaço
		cout << endl;
	}
}

// zera as restrições de todas as posições
void Futoshiki::resetRestrictions(){
	for(int i = 0; i < size_; ++i){
		for(int j = 0; j< size_; ++j){
			restrictions_[i][j] = 0;
		}                                                   
	}
}

// seta todos os dígitos como possíveis para todas as posições
void Futoshiki::resetPossibilities(){
	_9bits initial = ~0; // status inicial padrão, marca todos os dígitos válidos com 1 e inválidos com 0
	for(int i = MAX_SIZE-1; i > size_-1; --i){
		initial[i] = 0;
	}

	// seta as possibilidades de todos os quadrados com o status inicial
	for(int i = 0; i < size_; ++i){
		for(int j = 0; j< size_; ++j){
			possibilities_[i][j] = initial;
		}                                                   
	}
}


