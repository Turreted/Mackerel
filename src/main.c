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
    char *fen = "1k1q4/8/8/8/3N4/8/8/1K6 w - - 0 1";
    board_load_fen(&board, fen);

    Move moves[MAX_MOVES];

    int depth = 3;
    search.depth = depth;

    int score = dfs_helper(&search, &board, depth);
    char str[64];
    move_to_string(&(search.move), str);

    printf("Best move: %s with score %d\n", str, score);
    
    print_legal_moves(&board);
    board_print(&board);
    board_to_fen(&board);
    printf("\n");

    do_move(&board, &(search.move), &undo);
    board_print(&board);
    board_to_fen(&board);

}