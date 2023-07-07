#ifndef HTABLE_H
#define HTABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include "stdint.h"

#include "board.h"

#define MB_64_HMAP 26

typedef struct HashNode {
    bool set;
    u_int64_t board_hash;
    signed int value;
    struct HashNode *next;
} HashNode;

typedef struct {
    int size;
    int hash_bits;
    HashNode *map;
} HashMap;

HashMap *hashmap_init(int bits);
void hashmap_print(HashMap *hmap);
void hashmap_free(HashMap *hmap);
void hashmap_set(HashMap *hmap, Board *board, int value);
HashNode *hashmap_get(HashMap *hmap, Board *board);
u_int32_t hash_board(Board *board, int bits);

#endif