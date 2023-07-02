#include <stdio.h>
#include <stdlib.h>

#include "search.h"

#include "board.h"
#include "move.h"
#include "gen.h"
#include "eval.h"
#include "util.h"

#define INF 2147483640

int minmax(Search *search, Board *board, int depth) {
    search->depth = depth;
    int perspective = board->color == WHITE ? 1 : -1;
    return perspective * minmax_dfs(search, board, depth, -INF, INF);
}

int minmax_dfs(Search *search, Board *board, int depth, int alpha, int beta) {
    Move moves[MAX_MOVES];
    Undo undo;
    int move_count = gen_legal_moves(board, moves);

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

    // at the bottom of the tree return the raw board score
    if (depth == 0) {
        return eval_board(board);
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