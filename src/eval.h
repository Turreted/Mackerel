#ifndef EVAL_H
#define EVAL_H

#include "board.h"
#include "move.h"

typedef struct {
    int score;
    Move move;
} MoveScore;

int eval_board(Board *board);
void print_legal_moves(Board *board);
int gen_sorted_moves(Board *board, Move *moves);
int quiescence_search(Board *board, int alpha, int beta);

#endif