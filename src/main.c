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
    char *fen = "8/8/2q1k3/8/8/8/2N5/5K2 b - - 0 1";
    board_load_fen(&board, fen);

    Move moves[MAX_MOVES];

    int depth = 3;
    search.depth = depth;

    int move_count = gen_legal_moves(&board, moves);
    int score = dfs(&search, &board, depth);
    char str[64];
    move_to_string(&(search.move), str);

    printf("Best move: %s with score %d\n", str,score);
    
    print_legal_moves(&board);
    board_print(&board);
    board_to_fen(&board);
}