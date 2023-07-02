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

int main() {
    bb_init();
    char *fen = "r3k2r/p1ppqpb1/Bn3np1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b - - 0 1";
    board_load_fen(&board, fen);

    int depth = 4;
    search.depth = depth;

    int score = dfs_helper(&search, &board, depth);
    printf("Best move score: %d\n", score);
    
    print_legal_moves(&board);
    
    board_print(&board);
    board_print_fen(&board);
    printf("\n");

    do_move(&board, &(search.move), &undo);
    print_move(&board, &search.move);
    printf("\n");

    board_print(&board);
    board_print_fen(&board);
}