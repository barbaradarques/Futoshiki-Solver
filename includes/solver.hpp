#ifndef _SOLVER_HPP
#define _SOLVER_HPP

#include <iostream>

#define GREATER 2
#define LESS 1
#define NONE 0

void solve();
bool addNumber();
void printBoard();
void resetRestrictions();
bool isPossible(char num, char row, char col);
void setRestriction(int row1, int col1, int row2, int col2);
#endif
