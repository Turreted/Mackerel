#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "gen.h"
#include "eval.h"
#include "search.h"
#include "util.h"

static Board board;
static Undo undo;
static Search search;


int run_game(char *fen, int verbose) {
    board_load_fen(&board, fen);

    int depth = 5;
    search.depth = depth;

    // perform search
    clock_t begin = clock();
    Search search;
    char move_str[8];
    minmax(&search, &board, depth);
    move_to_string(&search.move, move_str);
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    if (verbose) {
        printf("Found best move %s with score %d\n", move_str, search.score);    
        printf("For Depth %d, checked %d positions in %fs\n", search.depth, search.eval_count, time_spent);    
        
        board_print(&board);
        board_print_fen(&board);
        printf("\n");

        do_move(&board, &search.move, &undo);
        printf("\n");

        board_print(&board);
        board_print_fen(&board);
    } else {
        //do_move(&board, &search.move, &undo);
        //board_print_fen(&board);
        print_move(&board, &search.move);
    }

    return 0;
}

void test_sorting(char *fen) {
    board_load_fen(&board, fen);

    Move moves[MAX_MOVES];
    int c = gen_legal_moves(&board, moves);
    order_moves(&board, moves, c);
    for (int i = 0; i < c; i++) {
        print_move(&board, &moves[i]);
        printf(" ");
    }
}

void test_eval(char *fen) {
    board_load_fen(&board, fen);
    int init = eval_board(&board);
    //quiescence_search(&board, -INF, +INF);

    printf("Final Score: %d\n", init);
}

// accept fen string as an input
int main(int argc, char **argv) {
    //int verbose = 0;
    char fen[256] = "";
    int verbose = 1;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
            verbose = 0;
        } else if (strlen(fen) + strlen(argv[i]) + 1 < 256) {
            strcat(fen, argv[i]);
            strcat(fen, " ");
        }
    }
    bb_init();
    run_game(fen, verbose);
}
