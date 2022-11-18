// Linked list implementation for virtual timers
//
// The list is singly linked and always sorted by the `timer_value` field

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app_error.h"
#include "nrf.h"
#include "nrf_delay.h"

#include "virtual_timer_linked_list.h"

// the linked list
static node_t* linked_list = NULL;


// -- External functions

// insert item into list sorted by `timer_value`
void list_insert(node_t* node) {

    // fault if given a NULL node
    if (node == NULL) {
        printf("\n***\nERROR: node passed into `list_insert` was NULL!!\n***\n");
    }
    else{
   	node_t* first = list_get_first();
    	node.next=first;
    	linked_list=node;  
    }
}


// return first element without removing
node_t* list_get_first() {
    return linked_list;
}


// remove last node in list
node_t* list_remove_last(){
  if (linked_list != NULL) {
    node_t* prev_node = linked_list;
    node_t* curr_node = linked_list->next;
    while(curr_node != NULL){
      prev_node = curr_node;
      curr_node = curr_node->next;
    }

    prev_node->next = NULL;
    return curr_node;
  }
}

// remove an arbitrary node if in list
void list_remove(node_t* node) {
    // fault if given a NULL node
    if (node == NULL) {
        printf("\n***\nERROR: node passed into `list_remove_node` was NULL!!\n***\n");
        nrf_delay_ms(100);
        APP_ERROR_CHECK(NRF_ERROR_NULL);
    }

    // check for empty list
    if (linked_list != NULL) {

        // check if this is the first node
        if (linked_list == node) {
            // it was the head, remove it
            linked_list = linked_list->next;
        } else {
            // it was not the head
            node_t* prev_niode = linked_list;
            node_t* curr_node = linked_list->next;
            while (curr_node != NULL && curr_node != node) {
                // iterate list until the end or the node we want
                prev_node = curr_node;
                curr_node = curr_node->next;
            }

            // remove the node if it exists
            if (curr_node != NULL) {
                prev_node->next = curr_node->next;
            }
        }
    }
}

// print contents of list
void list_print() {
    // handle an empty list
    if (linked_list == NULL) {
        printf("[ EMPTY ]\n");
    } else {

        // print first node
        printf("[ (%lu)", linked_list->timer_value);

        // print the other nodes
        node_t* curr_node = linked_list->next;
        while (curr_node != NULL) {
            printf(" -> (%lu)", curr_node->timer_value);
            curr_node = curr_node->next;
        }
        printf(" ]\n");
    }
}
