#include <stdio.h>

#include "board.h"
#include "gen.h"

// evaulation function to determine who is better at this position
int eval_board(Board *board) {
    signed int perspective = board->color == WHITE ? -1 : 1;
    return perspective * ( board->black_material - board->white_material );
}

void print_legal_moves(Board *board) {
    Move moves[MAX_MOVES];
    int count = gen_legal_moves(board, moves);
    printf("Legal move count: %d\n", count);
    for (int i = 0; i < count; i++) {
        Move *move = &moves[i];
        print_move(board, move);
        printf(", ");
    }
    printf("\n");
}