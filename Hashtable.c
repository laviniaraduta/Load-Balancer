/* Copyright 2021 Raduta Lavinia-Maria 313CA */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#include "Hashtable.h"

#define MAX_BUCKET_SIZE 64

int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

/*
 * Functie apelata dupa alocarea unui hashtable pentru a-l initializa.
 * Trebuie alocate si initializate si listele inlantuite.
 */
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *ht = (hashtable_t *)malloc(sizeof(hashtable_t));
	ht->size = 0;
	ht->hmax = hmax;
	ht->hash_function = hash_function;
	ht->compare_function = compare_function;
	ht->buckets = (linked_list_t **)malloc(hmax * sizeof(linked_list_t));
	for (unsigned int i = 0; i < hmax; i++) {
		ht->buckets[i] = ll_create(sizeof(struct info));
	}
	return ht;
}

/*
 * Although the key is sent as a void pointer (its type is not required)
 * when a new hashtable entry is created (if the key is not already
 * in the ht), a copy of the value at which key points and the address
 * of this copy must be saved in the info structure associated with the ht
 * entry. To know how many bytes need to be allocated and copied, use the
 * key_size_bytes parameter.
 * 
 * Motivation:
 * We need to copy the value to which key points because after a put call
 * (ht, key_actual, value_actual), the value that key_actual points to can
 * be altered (eg key_actual ++). If we used the address of the
 * key_actual pointer directly, it would practically change the key of an
 * entry in the hashtable from outside the hashtable. We do not want this,
 * because there is a risk of reaching the situation where we no longer
 * know which key a certain value is registered.
*/
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	int index = ht->hash_function(key) % ht->hmax;
	/* The list in which I have to search is ht->buckets[index] */
	ll_node_t *curr = ht->buckets[index]->head;
	if (ht_has_key(ht, key)) {
		while (curr) {
			if (ht->compare_function(key, ((struct info *)(curr->data))->key) == 0) {
				memcpy(((struct info *)(curr->data))->value, value, value_size);
				break;
			}
			curr = curr->next;
		}
	} else {
		struct info new_info;
		new_info.key = malloc(key_size);
		DIE(!new_info.key, "malloc failed!");
		memcpy(new_info.key, key, key_size);
		new_info.value = malloc(value_size);
		DIE(!new_info.value, "malloc failed!");
		memcpy(new_info.value, value, value_size);
		ll_add_nth_node(ht->buckets[index], ht->buckets[index]->size, &new_info);
		ht->size++;
	}
}

/*
 * Returns the value associated to the key sent by parameter.
 * If the key is not present, NULL is returned
*/
void *
ht_get(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *curr = ht->buckets[index]->head;
	while (curr) {
		if (ht->compare_function(key, ((struct info *)(curr->data))->key) == 0) {
			return ((struct info *)(curr->data))->value;
		}
		curr = curr->next;
	}
	return NULL;
}

/*
 * The function returns:
 * 1, if for the key sent as parameter was previously associated a value
 * 	  in the hashtable using function put.
 * 0, in other cases.
*/
int
ht_has_key(hashtable_t *ht, void *key)
{
	int index = ht->hash_function(key) % ht->hmax;
	ll_node_t *curr = ht->buckets[index]->head;
	while (curr) {
		if (ht->compare_function(key, ((struct info *)(curr->data))->key) == 0) {
			return 1;
		}
		curr = curr->next;
	}
	return 0;
}

/*
 * The function removes the associated key entry from the hashtable.
 * Attention to releasing all the memory used for a hashtable entry
 * (ie the memory for the key copy - see the note from the put procedure--,
 * for the info structure and for the Node structure in the linked list).
 */
void
ht_remove_entry(hashtable_t *ht, void *key)
{
	if (!ht_has_key(ht, key)) {
		printf("There is no entry with this key!\n");
	} else {
		int index = ht->hash_function(key) % ht->hmax, pos = 0;
		ll_node_t *curr = ht->buckets[index]->head, *to_remove;
		while (curr) {
			if (!ht->compare_function(key, ((struct info *)(curr->data))->key)) {
				break;
			}
			curr = curr->next;
			pos++;
		}
		to_remove = ll_remove_nth_node(ht->buckets[index], pos);
		free(((struct info *)(to_remove->data))->key);
		free(((struct info *)(to_remove->data))->value);
		free(to_remove->data);
		free(to_remove);
		ht->size--;
	}
}
/*
 * The function frees the memory used for all the entries in the hashtable,
 * then frees the memory used for the hashtable structure
*/
void
ht_free(hashtable_t *ht)
{
	ll_node_t *curr;
	for (unsigned int i = 0; i < ht->hmax; i++) {
		curr = ht->buckets[i]->head;
		while (curr) {
			free(((struct info *)(curr->data))->key);
			free(((struct info *)(curr->data))->value);
			curr = curr->next;
		}
		ll_free(&ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
