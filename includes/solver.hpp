#ifndef _SOLVER_HPP
#define _SOLVER_HPP

#include <iostream>
#include <bitset>

#define MAX_SIZE 9
#define BACKTRACKING 0 // backtracing sozinho
#define FORWARD_CHECKING 1 // backtracking com verificação adiante
#define MRV 2 // backtracking com verificação adiante e mínimos valores remanescentes
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

void solve(char l_method);
bool addNumber();
void printBoard();
void printRestrictions();
void resetRestrictions();
void resetPossibilities();
bool updatePossibilities(char num, int row, int col);
bool isValid(char num, int row, int col);
void setRestriction(int row1, int col1, int row2, int col2);
#endif