#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "search.h"
#include "board.h"
#include "ttable.h"
#include "move.h"
#include "gen.h"
#include "eval.h"
#include "util.h"

#include <time.h>
#include <stdlib.h>

#define Q_DEPTH 5
#define TTABLE_ENTRIES 1048576

int minmax(Search *search, Board *board, double time) {
  int cur_depth = 1;

  // init search strcut
  search->depth = cur_depth;
  search->finished = false;
  search->start_time = clock();
  search->time_limit = clock() + (time * CLOCKS_PER_SEC);
  search->eval_count = 0;
  search->ttable = ttable_init(TTABLE_ENTRIES);

  // track the best move generated by a complete search
  Move best_complete_search;
  
  int perspective = board->color == WHITE ? 1 : -1;
  int res = 0;

  // iterative deepening, limit by time not by depth
  while ((double) (clock() - search->start_time) / CLOCKS_PER_SEC < time) {
    res = perspective * minmax_dfs(search, board, cur_depth, -INF, INF);
    cur_depth++;
    search->depth = cur_depth;

    if (!search->finished) {
      best_complete_search = search->move;
      printf("Completed search of depth %d\n", cur_depth);
    }
  }

  //ttable_print(search->ttable);
  ttable_free(search->ttable);

  // overwrite search results with complete search result, as we do not know if the
  // current search result is good or garbage
  search->move = best_complete_search;

  return res;
}

/*
  perform minmax search with alpha-beta pruning. Note that each step
  returns the max possible score at that layer, regardless of player

  Alpha tracks the current best score, while beta tracks the opponent's
  best score. We can prune a branch if alpha >= beta, as we can assume
  that the opponent will never choose that branch
*/
int minmax_dfs(Search *search, Board *board, int depth, int alpha, int beta) {

  // kill search if our time limit is reached
  if (search->finished || clock() > search->time_limit) {
    search->finished = true;
    return beta;
  }

  // check transposition table for the current position, return it if it is returned
  int lookup_value = lookup_evaluation(search->ttable, board, depth, -beta, -alpha);
  if (lookup_value != LOOKUP_FAILED)
    return lookup_value;

  search->eval_count++;
  Move moves[MAX_MOVES];
  Undo undo;

  int hash_flag = HASHF_ALPHA;

  // get list of moves and sort them by predicted best to worst
  int move_count = gen_legal_moves(board, moves);
  order_moves(board, search, moves, move_count);

  // If there is an illegal move kill the program
  if (is_illegal(board)) {
    printf("Illegal board state reached!");
    exit(1);
  }


  // check if checkmate or stalemate
  if (move_count == 0) {
    if (is_check(board)) {
      ttable_set(search->ttable, board, -INF, depth, HASHF_EXACT);
      return -INF;
    } else {
      ttable_set(search->ttable, board, 0, depth, HASHF_EXACT);
      return 0;
    }
  }

  // at the bottom of the tree return the quiescence score, which is the
  // best reachable board state after all 'noisy' (ie captures + checks)
  // states have been reached
  if (depth == 0) {
    int qsearch = quiescence_search(board, Q_DEPTH, -INF, +INF);
    ttable_set(search->ttable, board, qsearch, depth, HASHF_EXACT);
    return qsearch;
  }

  for (int i = 0; i < move_count; i++) {
    // printf("alpha %d, beta: %d, depth: %d\n", alpha, beta, search->depth - depth);
    Move *move = &moves[i];

    do_move(board, move, &undo);

    // run dfs with the output value flipped, as each player wants the
    // opposite results also flip the alpha and beta parameters, as the
    // best play and worst plays are flipped (alpha: lower bound, beta: upper bound)
    int evaluation = -minmax_dfs(search, board, depth-1, -beta, -alpha);
    undo_move(board, move, &undo);

    // overwrite alpha value with our best score, write current move
    // to return value if we are at the top of our search tree
    if (evaluation > alpha && search->depth == depth && !search->finished) {
        printf("Move Found: ");
        print_move(board, move);
        printf("\n");
        search->move = *move;
        search->score = evaluation;

        // if we've found a checkmate, terminate our search
        if (search->score == INF) {
          printf("Checkmate Found!\n");
          search->finished = true;
        }
    }

    // previous move was too good, which means that this position will
    // never be reached (https://raphsilva.github.io/utilities/minimax_simulator/
    if (evaluation >= beta) {
      // record lookup for beta value
      ttable_set(search->ttable, board, beta, depth, HASHF_BETA);
      return beta; // *snip*
    }

    // overwrite alpha score if current is better
    if (evaluation > alpha) {
      alpha = evaluation;
      hash_flag = HASHF_EXACT;
    }
  }

  // record lookup for alpha-value
  ttable_set(search->ttable, board, alpha, depth, hash_flag);
  return alpha;
}

// search only noisy positions for the best viable position. This makes sure we
// don't stop our search in a bad position that the evaluator thinks is good
int quiescence_search(Board *board, int depth, int alpha, int beta) {
  if (is_checkmate(board)) {
    return -INF;
  }
  if (depth == 0) {
    return alpha;
  }

  Undo undo;
  Move moves[MAX_MOVES];
  int perspective = (board->color == WHITE) ? 1 : -1;
  int attacks = (board->color == WHITE) ?
                gen_white_attacks(board, moves) :
                gen_black_attacks(board, moves);

  // note that we have to multiply the result returned by eval so that
  // good black positions are also ranked as +
  int stand_eval = perspective * eval_board(board);
  if (stand_eval >= beta)
    return beta;
  alpha = max(alpha, stand_eval);

  for (int i = 0; i < attacks; i++) {
    Move *move = &moves[i];

    do_move(board, move, &undo);
    int evaluation = -quiescence_search(board, depth -1, -beta, -alpha);
    undo_move(board, move, &undo);

    // previous move was too good, prune
    if (evaluation >= beta) {
      return beta; // *snip*
    }

    // overwrite alpha score if current is better
    alpha = max(alpha, evaluation);
  }

  return alpha;
}