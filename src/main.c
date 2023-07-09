#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "gen.h"
#include "eval.h"
#include "search.h"
#include "ttable.h"
#include "util.h"

static Board board;
static Undo undo;
static Search search;


int run_game(char *fen, int verbose) {
  board_load_fen(&board, fen);

  int depth = 8;
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
    //ttable_print(search.hmap);
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
  //order_moves(&board, moves, c);
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

void ttable_test() {
  Board b1;
  Board b2;
  board_load_fen(&b1, "4k3/8/r7/8/8/8/3r4/5K2 w - - 0 1");
  board_load_fen(&b2, "4k3/p7/r7/8/8/8/3r4/5K2 w - - 0 1");
  TTable *h = ttable_init(100);
  printf("%llu\n", b1.hash);

  // set hash values
  ttable_set(h, &b1, 1, 0, HASHF_SET);
  ttable_set(h, &b2, 1, 0, HASHF_SET);
  ttable_print(h);

  // overwrite values
  ttable_set(h, &b1, 2, 1, HASHF_SET);
  ttable_print(h);

  ttable_free(h);
}

// accept fen string as an input
int main(int argc, char **argv) {
  char fen[256] = "";
  int verbose = 1;
  bb_init();

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
      verbose = 0;
    } else if (strlen(fen) + strlen(argv[i]) + 1 < 256) {
      strcat(fen, argv[i]);
      strcat(fen, " ");
    }
  }
  run_game(fen, verbose);
  //ttable_test();
}
