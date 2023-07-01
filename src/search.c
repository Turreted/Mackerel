#include <stdio.h>
#include <math.h>

#include "search.h"

#include "board.h"
#include "move.h"
#include "gen.h"
#include "eval.h"
#include "util.h"

int dfs(Search *search, Board *board, int depth) {
    Move moves[MAX_MOVES];
    Undo undo;
    int move_count = gen_moves(board, moves);

    // illegal move or checkmate is negative infinity
    if (is_illegal(board) || move_count == 0) {
        return -INFINITY;
    }

    // at the bottom of the tree return the raw board score
    if (depth == 0) {
        return eval_board(board);
    }

    int best_layer_score = -1000000;
    for (int i = 0; i < move_count; i++) {
        Move *move = &moves[i];

        do_move(board, move, &undo);

        int move_score = -dfs(search, board, depth - 1);
        if (move_score > best_layer_score) {
            best_layer_score = move_score;
            search->move = *move;
        }
        
        undo_move(board, move, &undo); 
    }

    return best_layer_score;
}