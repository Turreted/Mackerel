#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#include "board.h"
#include "gen.h"
#include "eval.h"
#include "search.h"

static Board board;
static Undo undo;
static Search search;


int run_game() {
    bb_init();
    char *fen = "4k3/7R/8/8/R7/8/8/4K3 w - - 0 1";
    board_load_fen(&board, fen);

    int depth = 4;
    search.depth = depth;

    // perform search
    clock_t begin = clock();
    Search search;
    char move_str[8];
    minmax(&search, &board, depth);
    move_to_string(&search.move, move_str);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("Found best move %s with score %d in %fs\n", move_str, search.score, time_spent);    
    
    
    board_print(&board);
    board_print_fen(&board);
    printf("\n");

    do_move(&board, &search.move, &undo);
    printf("\n");

    board_print(&board);
    board_print_fen(&board);

    return 0;
}

int main() {
    bb_init();
    char *fen = "4k3/1p6/8/8/R7/8/8/4K3 w - - 0 1";
    board_load_fen(&board, fen);

    Move moves[MAX_MOVES];
    gen_sorted_moves(&board, moves);
}




