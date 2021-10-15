/* Copyright 2021 Raduta Lavinia-Maria 313CA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"
#include "utils.h"

linked_list_t*
ll_create(unsigned int data_size)
{
    linked_list_t* list = (linked_list_t *)malloc(sizeof(linked_list_t));
	list->head = NULL;
    list->data_size = data_size;
	list->size = 0;
	return list;
}

/*
 * A new node is created, with the data sent by parameter and is added in
 * the list on the n-th position. The indexes start with 0. If n is greater
 * than the number of nodes, the new node is added at the end of the list.
 * If n < 0, error.
*/
void
ll_add_nth_node(linked_list_t* list, unsigned int n, const void* new_data)
{
    ll_node_t *new_node, *curr;
    unsigned int i = 0;

    new_node = (ll_node_t*)malloc(sizeof(ll_node_t));
    DIE(!new_node, "malloc failed!");
    new_node->data = malloc(list->data_size);
    DIE(!new_node->data, "malloc failed!");
    memcpy(new_node->data, new_data, list->data_size);

    if (n >= list->size) {
        n = list->size;
    }
    /* add at the begining of the list */
    if (n == 0) {
        new_node->next = list->head;
        list->head = new_node;
        list->size++;
    } else {
        curr = list->head;
        while (i  < n - 1 && curr->next != NULL) {
            curr = curr->next;
            i++;
        }
        new_node->next = curr->next;
        curr->next = new_node;
        list->size++;
    }
}

/*
 * Removes the node on the n-th position, sent as parameter.
 * The indexes start with 0. If n is greater than the number of nodes - 1,
 * the last node of the list is removed. If n < 0, error.
 * The function returns a pointer to the freshly removed node.
 * Freeing the memory for this node is the caller's responsability
*/
ll_node_t*
ll_remove_nth_node(linked_list_t* list, unsigned int n)
{
    ll_node_t *curr = list->head, *to_delete;
    unsigned int i = 0;
    /* if the list is empty */
    if (!list->head) {
        DIE(1, "error, list is empty!");
    } else {
        /* remove the first node */
        if (n == 0) {
            list->head = curr->next;
            list->size--;
            return curr;
        } else {
            i = 0;
            if (n > list->size) {
                n = list->size - 1;
            }
            while (i  < n - 1) {
                curr = curr->next;
                i++;
            }
            to_delete = curr->next;
            curr->next = to_delete->next;
            list->size--;
            return to_delete;
        }
    }
}

/*
 * The function returns the number of nodes in the list sent as paremeter
*/
unsigned int
ll_get_size(linked_list_t* list)
{
    return list->size;
}

/*
 * The function frees the memory used by all the nodes in the list and
 * then frees the memory used by the list structure. At the end,
 * the pointer sent as parameter is set at NULL (pointer at pointer)
*/
void
ll_free(linked_list_t** pp_list)
{
    ll_node_t *curr;
    while ((*pp_list)->head != NULL) {
        curr = (*pp_list)->head;
        (*pp_list)->head = curr->next;
        free(curr->data);
        free(curr);
    }
    free(*pp_list);
    pp_list = NULL;
}

/*
 * The function can only be used for lists that have data of type int!
 */
void
ll_print_int(linked_list_t* list)
{
    ll_node_t *curr = list->head;
    if (!list->head) {
        DIE(1, "error, list is empty!");
    } else {
        while (curr) {
            printf("%d ", *((int *)(curr->data)));
            curr = curr->next;
        }
    }
    printf("\n");
}

/*
 * The function can only be used for lists that have data of type string!
 */
void
ll_print_string(linked_list_t* list)
{
    ll_node_t *curr = list->head;
    if (!list->head) {
        DIE(1, "error, list is empty!");
    } else {
        while (curr) {
            printf("%s ", (char *)(curr->data));
            curr = curr->next;
        }
    }
    printf("\n");
}

/*
 * The function can only be used for lists that have data of type char!
 */
void
ll_print_char(linked_list_t* list)
{
    ll_node_t *curr = list->head;
    if (!list->head) {
        DIE(1, "error, list is empty!");
    } else {
        while (curr) {
            printf("%c ", *(char *)(curr->data));
            curr = curr->next;
        }
    }
    printf("\n");
}

