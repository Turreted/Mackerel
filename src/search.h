#ifndef SEARCH_H
#define SEARCH_H

#include <time.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "ttable.h"

typedef struct {
  int eval_count;
  int depth;
  int score;
  bool finished;
  Move move;
  clock_t start_time;
  clock_t time_limit;
  TTable *ttable;
} Search;

int minmax(Search *search, Board *board, double time);
int minmax_dfs(Search *search, Board *board, int depth, int alpha, int beta);
int quiescence_search(Board *board, int depth, int alpha, int beta);

#endif