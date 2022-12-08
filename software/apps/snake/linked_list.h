#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"

// a node within a linked list
typedef struct node_t {
    int x;
    int y;
    struct node_t* next;
} node_t;

void reset_list();

// Insert a node at the front of the linked list
void list_insert(node_t* node);

// Return the first node in the linked list without removing it. This value may
//  be NULL if the list is empty.
node_t* list_get_first();

node_t* list_remove_last();

// Remove the specified node from the linked list. Note that the memory for the
//  node is NOT automatically freed.
void list_remove(node_t* node);

void list_print();
