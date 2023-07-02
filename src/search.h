#ifndef SEARCH_H 
#define SEARCH_H

#include "board.h"
#include "move.h"

typedef struct {
    int depth;
    int score;
    Move move;
} Search;

int minmax(Search *search, Board *board, int depth);
int minmax_dfs(Search *search, Board *board, int depth, int alpha, int beta);

#endif