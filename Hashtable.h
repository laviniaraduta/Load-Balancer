/* Copyright 2021 Raduta Lavinia-Maria 313CA */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "LinkedList.h"

struct info {
	void *key;
	void *value;
};

typedef struct hashtable_t hashtable_t;
struct hashtable_t {
	linked_list_t **buckets; /* Array of linked lists. */
	/* Total number of existing nodes in all buckets */
	unsigned int size;
	unsigned int hmax; /* Number of buckets */
	/* (Pointer to) Hashing function for keys. */
	unsigned int (*hash_function)(void*);
	/* (Pointer to) Comparing function for 2 keys */
	int (*compare_function)(void*, void*);
};

hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*));

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size);

void *
ht_get(hashtable_t *ht, void *key);

int
ht_has_key(hashtable_t *ht, void *key);

void
ht_remove_entry(hashtable_t *ht, void *key);

unsigned int
ht_get_size(hashtable_t *ht);

unsigned int
ht_get_hmax(hashtable_t *ht);

void
ht_free(hashtable_t *ht);

int
compare_function_ints(void *a, void *b);

int
compare_function_strings(void *a, void *b);

#endif  /* HASHTABLE_H_ */
