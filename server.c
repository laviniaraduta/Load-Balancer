/* Copyright 2021 Raduta Lavinia-Maria 313CA */

#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "utils.h"

#define HMAX 10000

/*
 * Hashing functions for hashtable:
 */

static unsigned int
hash_function_string(void *a)
{
	/*
	 * Credits: http://www.cse.yorku.ca/~oz/hash.html
	 */
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c; /* hash * 33 + c */

	return hash;
}

server_memory* init_server_memory() {
	server_memory *server = calloc(1, sizeof(server_memory));
	DIE(!server, "malloc failed!");
	server->ht = ht_create(HMAX, hash_function_string, compare_function_strings);
	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	ht_put(server->ht, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server->ht, key);
}

char* server_retrieve(server_memory* server, char* key) {
	return (char *)(ht_get(server->ht, key));
}

void free_server_memory(server_memory* server) {
	ht_free(server->ht);
	free(server);
}
