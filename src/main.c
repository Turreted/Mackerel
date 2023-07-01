#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#include "board.h"
#include "gen.h"
#include "eval.h"
#include "search.h"

static Board board;
static Undo undo;
//static Search search;

int main() {
    bb_init();
    char *fen = "8/2r4P/1k6/6B1/5K2/8/8/8 w - - 0 1";
    board_load_fen(&board, fen);
    board_print(&board);
    printf("Board Color: %d\n", board.color);
    printf("\n");
    Move moves[MAX_MOVES];
    srand(time(NULL)); 

    int move_count = gen_moves(&board, moves);
    printf("%d moves of this type\n", move_count);

    for (int i = 0; i < move_count; i++){
        do_move(&board, &moves[i], &undo);
        board_print(&board);
        undo_move(&board, &moves[i], &undo);
    }
}