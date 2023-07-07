#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "stdint.h"

#include "board.h"
#include "hmap.h"


// Allocates a hashmap on the heap. The parameter bits corresonds to the size of
// each hash in bits, and thus the size of the table will be 2^bits
HashMap *hashmap_init(int bits) {
    HashMap *hmap = malloc(sizeof(HashMap));
    hmap->hash_bits = bits;
    hmap->size = 0x1 << bits;

    // note that we call sizeof(*hmap->map) to make an array of the size 
    // of the first element
    hmap->map = malloc(hmap->size * sizeof(hmap->map));

    for (int i = 0; i < hmap->size; i++) {
        HashNode hn;
        hn.board_hash = 0;

        hmap->map[i] = hn;
    }
    
    return hmap;
}

// TODO: fix.
void hashmap_free(HashMap *hmap) {
    for (int i = 0; i < hmap->size; i++) {
        HashNode *n = &hmap->map[i];
        
        while (n->next){
            n = n->next;
            free(n);
        }
    }

    free(hmap->map);
    free(hmap);
}

void hashmap_print(HashMap *hmap) {
    int size = 0;
    for (int i = 0; i < hmap->size; i++) {
        HashNode *node = &hmap->map[i];
        if (node->board_hash != 0) {
            printf("%llu: %d\n", node->board_hash, node->value);
            size++;

            while (node->next != NULL) {
                node = node->next;
                printf("%llu: %d\n", node->board_hash, node->value);
            }
        }
    }
    printf("Size: %f\n", ((double) size) / ((double) hmap->size));
}

// set a value in the hashmap, overwriting one if it already exists
void hashmap_set(HashMap *hmap, Board *board, int value) {
    int boardhash = hash_board(board, hmap->hash_bits);
    HashNode *node_p = &hmap->map[boardhash];

    // if bucket has not been initialized
    if (!node_p->set) {
        //printf("value: %d\n", value);
        node_p->board_hash = boardhash;
        node_p->value = value;
        node_p->set = true;

    // if bucket hash been initialized, see if entry exists. 
    // If the entry does not exist, add to tail.
    } else {
        // printf("Collision!\n");
        if (node_p->board_hash == boardhash) {
            node_p->value = value;
            return;
        }
        while (node_p->next != NULL) {
            if (node_p->board_hash == boardhash) {
                node_p->value = value;
                return;
            }
            node_p = node_p->next;
        }
        
        HashNode *new_node = malloc(sizeof(HashNode));
        new_node->board_hash = boardhash;
        new_node->value = value;
        new_node->set = true;

        node_p->next = new_node;
    }
}

// extract from hashmap, returns a 32-bit int where the first
// 16 bits represent if it is a hit and the last 16 represent the result
HashNode *hashmap_get(HashMap *hmap, Board *board) {
    int boardhash = hash_board(board, hmap->hash_bits);
    HashNode *node = &hmap->map[boardhash];

    // find correct node in bucket
    while (node->board_hash != boardhash && node->next != NULL)
        node = node->next;

    // hash miss vs hit: make more obvious
    return node;
}

// Computes a hash of a board by extracting the first n bits,
// depending on the size of the hashmap
u_int32_t hash_board(Board *board, int bits) {
    u_int32_t hashmask = 1;
    for (int i = 0; i < bits; i++)
        hashmask |= (0x1 << i);

    return (u_int32_t) hashmask & board->hash;
}
