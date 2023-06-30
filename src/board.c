#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "board.h"
#include "utils.h"


// UTILS

void display_bitboard(u_int64_t board) {
    for (int rc = 0; rc < 8; rc += 1) {
        unsigned char byte = (0xFF00000000000000LL & board) >> 56;
        printf("%s %s\n", bit_rep_pretty[byte >> 4], bit_rep_pretty[byte & 0x0F]);
        board <<= 8;
    }
}

int **make_board_array() {
    int **board;

    board = malloc(sizeof(int*) * 8);
    for (int i = 0; i < 8; i++)
        board[i] = malloc(sizeof(int) * 8);
    
    return board;
}

void free_board_array(int **board){
    for (int r = 0; r < 8; r++) {
        free(board[r]);
    }

    free(board);
}

u_int64_t array_to_bitboard(int** board) {
    u_int64_t bitboard = 0LL;

    for (int r = 0; r < 8; r++) {
        u_int8_t row = 0;
        for (int c = 0; c < 8; c++) {
            bool bit = board[r][c] == 0 ? 0 : 1;
            row <<= 1;
            row |= bit;
        }
        bitboard <<= 8;
        bitboard |= row;
    }

    return bitboard;
}

// movement mask creation

// Create a 64-bit mask of the possible squares that a rook can move to, 
// assuming that no other pieces are on the board
u_int64_t create_rook_movement_mask(int start_square) {
    int **rook_mask = make_board_array();

    int row = start_square / 8;
    int col = start_square % 8;

    for (int ci = 0; ci < 8; ci++) {
        rook_mask[row][ci] = 1;
    }

    for (int ri = 0; ri < 8; ri++) {
        rook_mask[ri][col] = 1;
    }

    rook_mask[row][col] = 0;

    u_int64_t bitboard = array_to_bitboard(rook_mask);
    free_board_array(rook_mask);

    return bitboard;
}

u_int64_t *create_all_blocker_bitboards

void generate_rook_lookup_table() {
    for (int start_square = 0; start_square < 64; start_square++) {
        create_rook_movement_mask(start_square);
    }
}
