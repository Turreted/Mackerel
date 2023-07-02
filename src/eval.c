#include <stdio.h>
#include <stdbool.h>

#include "board.h"
#include "gen.h"
#include "move.h"

// evaulation function to determine who is better at this position
int eval_board(Board *board) {
    signed int perspective = board->color == WHITE ? -1 : 1;
    return perspective * ( board->black_material - board->white_material );
}



// writes a list of sorted moves to the input struct
int gen_sorted_moves(Board *board, Move *moves) {
    return 0;
}

void print_legal_moves(Board *board) {
    Move moves[MAX_MOVES];
    int count = gen_legal_moves(board, moves);
    for (int i = 0; i < count; i++) {
        Move *move = &moves[i];
        print_move(board, move);
        printf(", ");
    }
    printf("\n");
}