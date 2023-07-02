#include <stdio.h>
#include <math.h>

#include "search.h"

#include "board.h"
#include "move.h"
#include "gen.h"
#include "eval.h"
#include "util.h"

#define INF 2147483647

int dfs_helper(Search *search, Board *board, int depth) {
    search->depth = depth;
    int perspective = board->color == WHITE ? 1 : -1;
    return perspective * dfs(search, board, depth, -INF, INF);
}

int dfs(Search *search, Board *board, int depth, int alpha, int beta) {
    Move moves[MAX_MOVES];
    Undo undo;
    int move_count = gen_legal_moves(board, moves);

    // illegal move or checkmate is negative infinity
    /*
    if (is_illegal(board) || move_count == 0) {
        return -INF;
    }*/

    // at the bottom of the tree return the raw board score
    if (depth == 0) {
        return eval_board(board);
    }

    for (int i = 0; i < move_count; i++) {
        printf("alpha %d, beta: %d, depth: %d\n", alpha, beta, search->depth - depth);
        Move *move = &moves[i];

        // compute dfs from moved board
        do_move(board, move, &undo);

        // note that we flip the parity of the returned value because we want 
        // the opposite of the other player
        // we also flip the alpha and beta inputs for the same reason, 
        // our worst position becomes our best, and uor best becomes our worst
        int evaluation = -dfs(search, board, depth - 1, -beta, -alpha);
        undo_move(board, move, &undo); 

        // exclude the rest of the search tree if this is better
        // than a previous best, as the opponent would not play it
        if (evaluation >= beta) {
          return beta;
        }

        // overwrite best score if current is better and we are at the top of
        // the search tree
        if (evaluation > alpha && search->depth == depth) {
          search->move = *move;
        }

        // overwrite alpha score if current is better
        alpha = max(alpha, evaluation);
    }

    return alpha;
}