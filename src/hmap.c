#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "stdint.h"

#include "board.h"
#include "hmap.h"


// Allocates a hashmap on the heap. The parameter bits corresonds to the size of
// each hash in bits, and thus the size of the table will be 2^bits
HashMap *hashmap_init(int size) {
    HashMap *hmap = malloc(sizeof(HashMap));
    hmap->size = size;

    // note that we call sizeof(*hmap->map) to make an array of the size of the first element
    hmap->map = malloc(hmap->size * sizeof(*hmap->map));

    for (int i = 0; i < hmap->size; i++) {
        HashNode hn;
        hn.flags = 0x0;
        hmap->map[i] = hn;
    }
    
    return hmap;
}

// TODO: fix.
void hashmap_free(HashMap *hmap) {
    free(hmap->map);
    free(hmap);
}

void hashmap_print(HashMap *hmap) {
    int size = 0;
    for (int i = 0; i < hmap->size; i++) {
        HashNode *node = &hmap->map[i];
        if (node->flags & HASHF_SET) {
            printf("%d: (%llu -> %d)\n", i, node->hash, node->value);
            size++;
        }
    }
    
    double filled = (size != 0) ? ((double) size) / ((double) hmap->size) : 0;
    printf("Size: %f\n", filled);
}

// set a value in the hashmap, overwriting with the deeper depth
void hashmap_set(HashMap *hmap, Board *board, int value, int depth) {
    u_int32_t boardkey = hash_board(board, hmap->size);

    HashNode *hn = &hmap->map[boardkey];
    hn->hash = board->hash;
    hn->flags = HASHF_SET;
    hn->depth = depth;
    hn->value = value;
}

// extract from hashmap, returns a 32-bit int where the first
// 16 bits represent if it is a hit and the last 16 represent the result
HashNode *hashmap_get(HashMap *hmap, Board *board) {
    int boardhash = hash_board(board, hmap->size);
    return &hmap->map[boardhash];
}

// Computes a hash of a board by extracting taking the mod of the hash function
// by our table size
u_int32_t hash_board(Board *board, int size) {
    return (u_int32_t) (board->hash % size);
}
