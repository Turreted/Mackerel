#ifndef SEARCH_H 
#define SEARCH_H

#include "board.h"
#include "move.h"
#include "hmap.h"

typedef struct {
    int eval_count;
    int depth;
    int score;
    Move move;
    HashMap *hmap;
} Search;

int minmax(Search *search, Board *board, int depth);
int minmax_dfs(Search *search, Board *board, int depth, int alpha, int beta);
int quiescence_search(Board *board, int depth, int alpha, int beta);

#endif