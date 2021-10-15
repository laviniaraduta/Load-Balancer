/* Copyright 2021 Raduta Lavinia-Maria 313CA */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "load_balancer_utils.h"
#include "load_balancer.h"

unsigned int
hash_function_servers(void *a)
{
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int
hash_function_key(void *a)
{
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}


void
add_new_value(int *v, int size, int value)
{
    int pos = size;
    for (int i = 0; i < size; i++) {
        if (hash_function_servers(&value) <
            hash_function_servers(&v[i])) {
            pos = i;
            break;
        }
    }
    /* shifting positions to make room for the new value */
    for (int i = size; i > pos; i--) {
        v[i] = v[i - 1];
    }
    v[pos] = value;
}

server_memory*
find_server(load_balancer *main, int tag, int *index)
{
    for (int i = 0 ; i < main->servers_count; i++) {
        if (main->servers[i]->id == tag % MAX_SERVERS) {
            *index = i;
            return main->servers[i];
        }
    }
    return NULL;
}

void
find_keys(server_memory* server, server_memory* next_server,
    unsigned int next_hash, unsigned int prev_hash, int i)
{
    char *value, *key;
    for (unsigned int k = 0; k < next_server->ht->hmax; k++) {
        ll_node_t *curr = next_server->ht->buckets[k]->head;
        while (curr) {
            key = (char *)(((struct info *)(curr->data))->key);
            unsigned int key_hash = hash_function_key(key);
            unsigned int server_hash = hash_function_servers(&server->tags[i]);
            /* the replica is first on the hashring */
            if (server_hash < next_hash && next_hash < prev_hash) {
                if (key_hash > prev_hash || key_hash < server_hash) {
                    value =(char *)(((struct info *)(curr->data))->value);
                    server_store(server, key, value);
                    curr = curr->next;
                    server_remove(next_server, key);
                } else {
                    curr = curr->next;
                }
            } else {
                if (key_hash > prev_hash && key_hash < server_hash) {
                    value = (char *)(((struct info *)(curr->data))->value);
                    server_store(server, key, value);
                    curr = curr->next;
                    server_remove(next_server, key);
                } else {
                    curr = curr->next;
                }
            }
        }
    }
}

void
find_neighbor_indexes(load_balancer* main, server_memory* server,
    int j, int *prev_index, int *next_index)
{
    if (j == 0) {
        *prev_index = 3 * main->servers_count - 1;
    } else {
        *prev_index = j - 1;
    }
    if (j == 3 * main->servers_count - 1) {
        *next_index = 0;
    } else {
        *next_index = j + 1;
    }
    while (main->hashring[*next_index] % MAX_SERVERS == server->id
            && main->servers_count > 1) {
        (*next_index)++;
        if (*next_index == 3 * main->servers_count) {
            *next_index = 0;
        }
    }
}

void
distribute_products_add(load_balancer* main,
    server_memory* server, int i)
{
    server_memory* next_server;
    unsigned int prev_hash, next_hash;
    int prev_index, next_index, pos;
    for (int j = 0 ; j < 3 * main->servers_count; j++) {
        if (main->hashring[j] == server->tags[i]) {
            find_neighbor_indexes(main, server, j, &prev_index, &next_index);
            next_server = find_server(main, main->hashring[next_index], &pos);
            next_hash = hash_function_servers(&(main->hashring[next_index]));
            prev_hash = hash_function_servers(&(main->hashring[prev_index]));
            if (next_server->ht->size) {
                find_keys(server, next_server, next_hash, prev_hash, i);
            }
        }
    }
}

void
find_neighbor_tags_remove(load_balancer* main, server_memory* server,
    int* pos, int* next_tag, int* prev_tag)
{
    for (int i = 0; i < 3; i++) {
        int j = pos[i] - 1;
        if (j < 0) {
            j = 3 * main->servers_count - 1;
        }
        /* Skipping the replicas of the same server */
        while (server->id == main->hashring[j] % MAX_SERVERS) {
            j--;
            if (j < 0) {
                j = 3 * main->servers_count + j;
            }
        }
        prev_tag[i] = main->hashring[j];
        j = (pos[i] + 1) % (3 * main->servers_count);
        while (server->id == main->hashring[j] % MAX_SERVERS) {
            j = (j + 1) % (3 * main->servers_count);
        }
        next_tag[i] = main->hashring[j];
    }
}

void
remove_from_hashring(load_balancer* main, server_memory *server,
    int *next_tag, int *prev_tag)
{
    int pos[3] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = pos[i]; j < 3 * main->servers_count; j++) {
            if (server->tags[i] == main->hashring[j]) {
                pos[i] = j;
            }
        }
    }
    /* Sorting the positions of the replicas in descending order */
    for (int i = 0; i < 2; i++) {
        for (int j = i + 1; j < 3; j++) {
            if (pos[i] < pos[j]) {
                int aux = pos[i];
                pos[i] = pos[j];
                pos[j] = aux;
            }
        }
    }
    find_neighbor_tags_remove(main, server, pos, next_tag, prev_tag);
    for (int i = 0; i < 3; i++) {
        for (int j = pos[i]; j < 3 * main->servers_count - i - 1; j++) {
            main->hashring[j] = main->hashring[j + 1];
        }
    }
}

void
remove_from_server_arr(load_balancer *main, int index)
{
    for (int i = index; i < main->servers_count; i++) {
        main->servers[i] = main->servers[i + 1];
    }
}

void
distribute_products_remove(load_balancer* main, server_memory* server,
    int prev, int next)
{
    int index;
    server_memory *next_server = find_server(main, next, &index);
    unsigned int next_hash = hash_function_servers(&next);
    unsigned int prev_hash = hash_function_servers(&prev);
    for (unsigned int k = 0; k < server->ht->hmax; k++) {
        ll_node_t *curr = server->ht->buckets[k]->head;
        while (curr) {
            char *key = ((struct info *)(curr->data))->key;
            unsigned int key_hash = hash_function_key(key);
            /* the replica is first or last on the hashring */
            if (next_hash < prev_hash) {
                if (key_hash < next_hash || key_hash > prev_hash) {
                    char *value =
                    (char *)(((struct info *)(curr->data))->value);
                    server_store(next_server, key, value);
                    curr = curr->next;
                    server_remove(server, key);
                } else {
                    curr = curr->next;
                }

            } else {
                if (key_hash < next_hash && key_hash > prev_hash) {
                    char *value =
                    (char *)(((struct info *)(curr->data))->value);
                    curr = curr->next;
                    server_store(next_server, key, value);
                    server_remove(server, key);
                } else {
                    curr = curr->next;
                }
            }
        }
    }
}
