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

#define LOOKUP_FAILED -1

// Thank you to https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm

 typedef struct {
    u_int64_t hash;
    int depth; // how many ply were searched ahead of eval
    int flags;
    int value;
    Move best;
} TableEntry;

typedef struct {
    int size;
    TableEntry *map;
} TTable;

TTable *ttable_init(int size);
void ttable_print(TTable *ttable);
void ttable_free(TTable *ttable);
void ttable_set(TTable *ttable, Board *board, int value, int depth, int hash_flags);
TableEntry *ttable_get(TTable *ttable, Board *board);
u_int32_t hash_board(Board *board, int size);
int lookup_evaluation(TTable *ttable, Board *board, int depth, int alpha, int beta);

#endif