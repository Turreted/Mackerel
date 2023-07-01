#ifndef SEARCH_H 
#define SEARCH_H

#include "board.h"
#include "move.h"

typedef struct {
    int score;
    Move move;
} Search;

int dfs(Search *search, Board *board, int depth);

#endif