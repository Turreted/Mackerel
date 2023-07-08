#ifndef HTABLE_H
#define HTABLE_H

#include <stdlib.h>
#include <stdbool.h>
#include "stdint.h"

#include "move.h"
#include "board.h"

#define HASHF_NULL 0x0
#define HASHF_SET 0x1
#define HASHF_EXACT 0x2
#define HASHF_ALPHA 0x4
#define HASHF_BETA 0x8

// Thank you to https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm

 typedef struct {
    u_int64_t hash;
    int depth;
    int flags;
    int value;
    Move best;
} HashNode;

typedef struct {
    int size;
    HashNode *map;
} HashMap;

HashMap *hashmap_init(int size);
void hashmap_print(HashMap *hmap);
void hashmap_free(HashMap *hmap);
void hashmap_set(HashMap *hmap, Board *board, int value, int depth);
HashNode *hashmap_get(HashMap *hmap, Board *board);
u_int32_t hash_board(Board *board, int size);

#endif