#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "stdint.h"

#include "board.h"
#include "ttable.h"


// Allocates a transposition table on the heap with $size entries
TTable *ttable_init(int size) {
    TTable *ttable = malloc(sizeof(TTable));
    ttable->size = size;

    // note that we call sizeof(*ttable->map) to make an array of the size of the first element
    ttable->map = malloc(ttable->size * sizeof(*ttable->map));

    for (int i = 0; i < ttable->size; i++) {
        TableEntry hn;
        hn.flags = 0x0;
        ttable->map[i] = hn;
    }
    
    return ttable;
}

void ttable_free(TTable *ttable) {
    free(ttable->map);
    free(ttable);
}

void ttable_print(TTable *ttable) {
    int size = 0;
    for (int i = 0; i < ttable->size; i++) {
        TableEntry *node = &ttable->map[i];
        // only print nodes that are set
        if (node->flags != 0x0) {
            printf("%d: (%llu -> %d)\n", i, node->hash, node->value);
            size++;
        }
    }
    
    double filled = (size != 0) ? ((double) size) / ((double) ttable->size) : 0;
    printf("Size: %f\n", filled);
}

// set a value in the ttable, overwriting with the deeper depth
void ttable_set(TTable *ttable, Board *board, int value, int depth, int hash_flags) {
    u_int32_t boardkey = hash_board(board, ttable->size);
    TableEntry *tentry = &ttable->map[boardkey];

    // overwrite if search depth is greater
    if (depth >= tentry->depth) {
        tentry->hash = board->hash;
        tentry->flags = hash_flags;
        tentry->depth = depth;
        tentry->value = value;
    }
}

// extract from ttable, returns a 32-bit int where the first
// 16 bits represent if it is a hit and the last 16 represent the result
TableEntry *ttable_get(TTable *ttable, Board *board) {
    int boardhash = hash_board(board, ttable->size);
    return &ttable->map[boardhash];
}

// reterieve an evalutaion score based on the input Alpha and Beta parametera
// https://github.com/SebLague/Chess-AI/blob/main/Assets/Scripts/Core/TranspositionTable.cs#L24
int lookup_evaluation(TTable *ttable, Board *board, int depth, int alpha, int beta) {
    // get entry from board 
    TableEntry *tentry = ttable_get(ttable, board);
    
    // if entry does not exist or is of shallower depth
    if (!tentry->flags || depth > tentry->depth) return LOOKUP_FAILED;

    // return the score stored in the transposition table if it is
    // deeper than the current search and has viable AB scores

    // if we've stored the exact evaluation for this position, return it
    if (tentry->flags == HASHF_EXACT) return tentry->value;

    // if alpha score, return the max of the current alpha and the table's stored alpha.
    // The table's alpha functions as a lower bound
    if (tentry->flags == HASHF_ALPHA && tentry->value > alpha) return alpha;

    // if beta score, the table's beta functions as a lower bound
    if (tentry->flags == HASHF_BETA && tentry->value < beta) return beta;

    return LOOKUP_FAILED;
}

// Computes a hash of a board by extracting taking the mod of the hash function
// by our table size
u_int32_t hash_board(Board *board, int size) {
    return (u_int32_t) (board->hash % size);
}
