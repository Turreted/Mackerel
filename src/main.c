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
    char *fen = "rnbqkbnr/pRRRRRRp/8/1RRRRRR1/1RRR3R/5R2/PPP1PRPP/1N4N1 w - - 0 1";
    board_load_fen(&board, fen);

    Move moves[MAX_MOVES];
    srand(time(NULL)); 

    int move_count = gen_moves(&board, moves);

    board_print(&board);
    board_to_fen(&board);
}