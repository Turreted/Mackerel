#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "search.h"
#include "board.h"
#include "hmap.h"
#include "move.h"
#include "gen.h"
#include "eval.h"
#include "util.h"

#define Q_DEPTH 5

int minmax(Search *search, Board *board, int depth) {
    search->depth = depth;
    search->eval_count = 0;
    search->hmap = hashmap_init(16);

    int perspective = board->color == WHITE ? 1 : -1;
    int res = perspective * minmax_dfs(search, board, depth, -INF, INF);

    //hashmap_print(search->hmap);
    hashmap_free(search->hmap);

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
    search->eval_count++;
    Move moves[MAX_MOVES];
    Undo undo;

    int hash_flags = HASHF_ALPHA;

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
        hashmap_set(search->hmap, board, -INF, search->depth - depth);
        return -INF;
      } else {
        hashmap_set(search->hmap, board, -INF, search->depth - depth);
        return 0;
      }
    }

    // at the bottom of the tree return the quiescence score, which is the 
    // best reachable board state after all 'noisy' (ie captures + checks)
    // states have been reached
    if (depth == 0) {
        hashmap_set(search->hmap, board, -INF, search->depth - depth);
        return quiescence_search(board, Q_DEPTH, -INF, +INF);
    }

    for (int i = 0; i < move_count; i++) {
        // printf("alpha %d, beta: %d, depth: %d\n", alpha, beta, search->depth - depth);
        Move *move = &moves[i];

        do_move(board, move, &undo);

        // run dfs with the output value flipped, as each player wants the 
        // opposite results also flip the alpha and beta parameters, as the 
        // best play and worst plays are flipped
        int evaluation = -minmax_dfs(search, board, depth-1, -beta, -alpha);
        undo_move(board, move, &undo);
        
        // previous move was too good, which means that this position will
        // never be reached (https://raphsilva.github.io/utilities/minimax_simulator/
        if (evaluation >= beta) {
          return beta; // *snip*
        }

        // overwrite alpha value with our best score, write current move
        // to return value if we are at the top of our search tree
        if (evaluation > alpha && search->depth == depth) {
          //printf("Alpha: %d, Beta: %d\n", alpha, beta);
          search->move = *move;
          search->score = evaluation;
        }

        // overwrite alpha score if current is better
        alpha = max(alpha, evaluation);
    }

    return alpha;
}

// search only noisy positions for the best viable position. This makes sure we
// don't stop our search in a bad position that the evaluator thinks is good
int quiescence_search(Board *board, int depth, int alpha, int beta) {
  // probably buggy, might want to return max of alpha and eval_board?
  if (is_checkmate(board)) { 
    return -INF + 1;
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