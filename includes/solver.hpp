#ifndef _SOLVER_HPP
#define _SOLVER_HPP

#include <iostream>
#include <bitset>
#include <chrono>
#include <utility> 
#include <queue>
#include <sstream>
#include <fstream>

#define MAX_SIZE 9
// #define MAX_CALLS 1000000

// Flags que determinam as técnicas a serem utilizadas
#define BACKTRACKING '1' // backtracking sozinho
#define FORWARD_CHECKING '2' // backtracking com verificação adiante
#define MRV '3' // backtracking com verificação adiante e mínimos valores remanescentes

// Flags armazenadas em cada byte da matriz de restrições
#define GTR_THAN_TOP 1
#define LS_THAN_TOP (1 << 1)
#define GTR_THAN_BOTTOM (1 << 2)
#define LS_THAN_BOTTOM (1 << 3)
#define GTR_THAN_RIGHT (1 << 4)
#define LS_THAN_RIGHT (1 << 5)
#define GTR_THAN_LEFT (1 << 6)
#define LS_THAN_LEFT (1 << 7)


using namespace std;

typedef bitset<9> _9bits;
typedef	pair<char, size_t> _pair;

void solve(char l_method, ofstream& file);
bool addNumber();
void printBoard();
void printRestrictions();
void resetRestrictions();
void resetPossibilities();
bool updatePossibilities(char num, int row, int col);
bool isValid(char num, int row, int col);
void setRestriction(int row1, int col1, int row2, int col2);
#endif
