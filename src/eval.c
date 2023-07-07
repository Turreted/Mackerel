#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "gen.h"
#include "move.h"
#include "util.h"
#include "eval.h"
#include "hmap.h"
#include "search.h"

// evaulation function to determine who is better at which position.
// Should return negative values for black and positive values for white
int eval_board(Board *board) {
    int perspective = board->color == WHITE ? 1 : -1;

    // i need this for some reason
    if (is_checkmate(board)) {
        return -perspective * INF;
    }

    // find points resulting from best position
    int position_weight = 0;
    for (int sq = 0; sq < 64; sq++) {
        int color = COLOR(board->squares[sq]);
        
        if (color == WHITE) {
            switch (PIECE(board->squares[sq])){
                case PAWN: position_weight   += POSITION_WHITE_PAWN[sq]; break;
                case KNIGHT: position_weight += POSITION_WHITE_KNIGHT[sq]; break;
                case BISHOP: position_weight += POSITION_WHITE_BISHOP[sq]; break;
                case ROOK: position_weight   += POSITION_WHITE_ROOK[sq]; break;
                case QUEEN: position_weight  += POSITION_WHITE_QUEEN[sq]; break;
                case KING: position_weight   += POSITION_WHITE_KING[sq]; break;
                default: break;
            }
        } else if (color == BLACK) {
            switch (PIECE(board->squares[sq])){
                case PAWN: position_weight   -= POSITION_BLACK_PAWN[sq]; break;
                case KNIGHT: position_weight -= POSITION_BLACK_KNIGHT[sq]; break;
                case BISHOP: position_weight -= POSITION_BLACK_BISHOP[sq]; break;
                case ROOK: position_weight   -= POSITION_BLACK_ROOK[sq]; break;
                case QUEEN: position_weight  -= POSITION_BLACK_QUEEN[sq]; break;
                case KING: position_weight   -= POSITION_BLACK_KING[sq]; break;
                default: break;
            }
        }
    }

    // find point difference in material
    int material_weight = board->white_material - board->black_material;

    //printf("Position weight: %d, material weight: %d\n", position_weight, material_weight);

    return (position_weight + material_weight);
}

// compares move scores so we can sort the list. Compares from greatest to least.
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
int order_moves(Board *board, Search *search, Move *movelist, int move_count) {
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

        int predicted_score = 0;

        // check if position is in the hash table, in which case use it for move ordering
        HashNode *ttlookup = hashmap_get(search->hmap, board);
        if (false) {
            //printf("Hash Hit!\n");
            //int perspective = (board->color == WHITE) ? 1 : -1;
            predicted_score = ttlookup->value;
        } else {
            // taking a high-value piece with a low-value piece is good
            predicted_score += max(capture_value - piece_value + 10, 0);

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
        }
        if (ttlookup->set) {
            printf("\nScore: %d vs %d\n", predicted_score, ttlookup->value);
            board_print(board);
        }

        // add score to list to it can be sorted
        MoveScore s;
        s.move = *move;
        s.score = predicted_score;
        move_scores[i] = s;
    }

    qsort(move_scores, move_count, sizeof(MoveScore), comp_moves);

    for (int i = 0; i < move_count; i++){
       // TODO: make sure there isn't a memory corruption issue here
       movelist[i] = move_scores[i].move;
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