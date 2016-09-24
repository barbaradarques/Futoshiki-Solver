#ifndef _SOLVER_HPP
#define _SOLVER_HPP

#include <iostream>

#define MAX_SIZE 9

// Flags armazenadas em cada byte da matriz de restrições
#define GTR_THAN_TOP 1
#define LS_THAN_TOP (1 << 1)
#define GTR_THAN_BOTTOM (1 << 2)
#define LS_THAN_BOTTOM (1 << 3)
#define GTR_THAN_RIGHT (1 << 4)
#define LS_THAN_RIGHT (1 << 5)
#define GTR_THAN_LEFT (1 << 6)
#define LS_THAN_LEFT (1 << 7)


void solve();
bool addNumber();
void printBoard();
void printRestrictions();
void resetRestrictions();
bool isValid(char num, int row, int col);
void setRestriction(int row1, int col1, int row2, int col2);
#endif
