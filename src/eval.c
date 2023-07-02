#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "gen.h"
#include "move.h"
#include "util.h"
#include "eval.h"

#define INF 99999

// evaulation function to determine who is better at this position
int eval_board(Board *board) {
    signed int perspective = board->color == WHITE ? -1 : 1;
    return perspective * ( board->black_material - board->white_material );
}

// compares move scores so we can sort the list. Sort from greatest to least.
int comp_moves(const void * a, const void * b) {
    MoveScore *x = (MoveScore *) a;
    MoveScore *y = (MoveScore *) b;
    return y->score - x->score;
}

// convert piece to value
int score_piece_value(int piece) {
    switch (piece){
        case PAWN:
            return MATERIAL_PAWN;
        case KNIGHT:
            return MATERIAL_KNIGHT;
        case BISHOP:
            return MATERIAL_BISHOP;
        case ROOK:
            return MATERIAL_ROOK;
        case QUEEN:
            return MATERIAL_QUEEN;
        case KING:
            return MATERIAL_KING;
    }

    return 0;
}

// Sorts moves by (predicted) best to worst to speed up Alpha-Beta pruning
int gen_sorted_moves(Board *board, Move *moves) {
    Move movelist[MAX_MOVES];
    int move_count = gen_legal_moves(board, movelist);

    MoveScore move_scores[move_count];

    for (int i = 0; i < move_count; i++) {
        Undo undo;
        Move *move = &movelist[i];

        int src = movelist[i].src;
        int dst = movelist[i].dst;
        int piece = PIECE(board->squares[src]);
        int piece_value = score_piece_value(piece);

        int capture = PIECE(board->squares[dst]);
        int capture_value = score_piece_value(capture);

        unsigned int predicted_score = 0;

        // taking a high-value piece with a low-value piece is good
        predicted_score += max(capture_value - piece_value, 0);

        // promoting a pawn is good
        predicted_score += move->promotion * 500;

        // putting the opponent in check is good, checkmate is better
        do_move(board, move, &undo);
        predicted_score += is_check(board) * 50;
        predicted_score += is_checkmate(board) * INF;
        undo_move(board, move, &undo);

        // moving a piece to a position attacked by a pawn is bad
        bb dst_mask = 0x1L << dst;
        Move m[MAX_MOVES];
        int pawn_attack = board->color == WHITE ? 
            gen_black_pawn_attacks_against(board, m, dst_mask) : 
            gen_white_pawn_attacks_against(board, m, dst_mask);
        predicted_score += pawn_attack * (MATERIAL_PAWN - piece_value);

        // add score to list to it can be sorted
        MoveScore s;
        s.move = *move;
        s.score = predicted_score;
        move_scores[i] = s;
    }

    qsort(move_scores, move_count, sizeof(MoveScore), comp_moves);

    for (int i = 0; i < move_count; i++){
        /*
        char move_str[8];
        move_to_string(&move_scores[i].move, move_str);
        printf("Move: %s, score: %d\n", move_str, move_scores[i].score);
        */
       // TODO: make sure there isn't a memory corruption issue here
       moves[i] = move_scores[i].move;
    }

    return move_count;
}

void print_legal_moves(Board *board) {
    Move moves[MAX_MOVES];
    int count = gen_legal_moves(board, moves);
    for (int i = 0; i < count; i++) {
        Move *move = &moves[i];
        print_move(board, move);
        printf(", ");
    }
    printf("\n");
}