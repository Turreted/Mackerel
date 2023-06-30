#include <stdio.h>

#include "board.h"
#include "gen.h"

static Board board;
Move moves[MAX_MOVES];

int main() {
    char *fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    board_load_fen(&board, fen);
    board_print(&board);
    printf("Board Color: %d\n", board.color);

    int count = gen_legal_moves(&board, moves);
    for (int i = 0; i < count; i++) {
        Move *move = &moves[i];
        print_move(&board, move);
        printf("\n");
    }
}