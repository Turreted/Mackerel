#include <stdio.h>
#include <stdlib.h>

#include "search.h"

#include "board.h"
#include "move.h"
#include "gen.h"
#include "eval.h"
#include "util.h"

#define INF 2147483646

int minmax(Search *search, Board *board, int depth) {
    search->depth = depth;
    search->eval_count = 0;
    int perspective = board->color == WHITE ? 1 : -1;
    return perspective * minmax_dfs(search, board, depth, -INF, INF);
}

// perform minmax search with alpha-beta pruning. Note that each step
// returns the max possible score at that layer, regardless of player
int minmax_dfs(Search *search, Board *board, int depth, int alpha, int beta) {
    search->eval_count++;
    Move moves[MAX_MOVES];
    Undo undo;
    int move_count = gen_sorted_moves(board, moves);

    // If there is an illegal move kill the program
    if (is_illegal(board)) {
      printf("Illegal board state reached!");
      exit(1);
    }

    // check if checkmate or stalemate
    if (move_count == 0) {
      if (is_check(board)) {
        return -INF;
      } else {
        return 0;
      }
    }

    // at the bottom of the tree return the quiescence score, which is the 
    // best reachable board state after all 'noisy' (ie captures + checks)
    // states have been reached
    if (depth == 0) {
        return quiescence_search(board, -INF, +INF);
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

        // overwrite alpha value with our best score, write current move
        // to return value if we are at the top of our search tree
        if (evaluation > alpha && search->depth == depth) {
          search->move = *move;
          search->score = evaluation;
        }

        // overwrite alpha score if current is better
        alpha = max(alpha, evaluation);
        
        // previous move was too good, which means that this position will
        // never be reached (https://raphsilva.github.io/utilities/minimax_simulator/
        if (evaluation >= beta) {
          return beta; // *snip*
        }
    }

    return alpha;
}

// search only captures for the best viable position. This makes sure we
// don't stop our search in a bad position that the evaluator thinks is good
int quiescence_search(Board *board, int alpha, int beta) {
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
      if (!(board->squares[move->dst] == KING)) {

        do_move(board, move, &undo);
        int evaluation = -quiescence_search(board, -beta, -alpha);
        undo_move(board, move, &undo);
        
        // previous move was too good, prune
        if (evaluation >= beta) {
          return beta; // *snip*
        }

        // overwrite alpha score if current is better
        alpha = max(alpha, evaluation);
      }
    }
  
  return alpha;
}