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
    char *fen = "r2qkb1r/p1p2ppp/2p5/3p4/4p3/2N2b2/PPPPQPPP/R1B2RK1 w - - 0 1";
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

    printf("Found best move %s with score %d\n", move_str, search.score);    
    printf("For Depth %d, checked %d positions in %fs\n", search.depth, search.eval_count, time_spent);    
    
    board_print(&board);
    board_print_fen(&board);
    printf("\n");

    do_move(&board, &search.move, &undo);
    printf("\n");

    board_print(&board);
    board_print_fen(&board);

    return 0;
}

void test_sorting() {
    bb_init();
    char *fen = "4k3/1p6/8/8/R7/8/8/4K3 w - - 0 1";
    board_load_fen(&board, fen);

    Move moves[MAX_MOVES];
    gen_sorted_moves(&board, moves);
}

void test_eval() {
    bb_init();
    char *fen = "8/8/8/8/8/8/3p4/8 w - - 0 1";
    board_load_fen(&board, fen);
    Move moves[256];
    printf("Pawn moves: %d\n", gen_white_pawn_moves(&board, moves));

    eval_board(&board);
}

int main() {
    run_game();
}
