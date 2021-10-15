/* Copyright 2021 Raduta Lavinia-Maria 313CA */

#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"
#include "load_balancer_utils.h"
#include "utils.h"
#include "Hashtable.h"

#define MAX_SERVERS 100000

load_balancer*
init_load_balancer()
{
	load_balancer *lb = (load_balancer *)calloc(1, sizeof(load_balancer));
    DIE(!lb, "malloc failed");
    lb->servers_count = 0;
    for (int i = 0 ; i < 3 * MAX_SERVERS; i++) {
        lb->hashring[i] = 0;
    }
    return lb;
}

void
loader_store(load_balancer* main, char* key, char* value, int* server_id)
{
	int found = 0, index;
    unsigned int server_hash, key_hash;
    server_memory* server;
    for (int i = 0; i < 3 * main->servers_count; i++) {
        server_hash = hash_function_servers(&(main->hashring[i]));
        key_hash = hash_function_key(key);
        if (server_hash > key_hash){
            server = find_server(main, main->hashring[i], &index);
            server_store(server, key, value);
            *server_id = server->id;
            found = 1;
            break;
        }
    }
    /* 
     * if the server was not already found,
     * then the correspondent server is the first one (index 0)
    */
    if (!found) {
        server_memory *server = find_server(main, main->hashring[0], &index);
        server_store(server, key, value);
        *server_id = server->id;
    }
}

char*
loader_retrieve(load_balancer* main, char* key, int* server_id)
{
	int ok = 0, index;
    unsigned int server_hash, key_hash;
    server_memory* server;
    for (int i = 0; i < 3 * main->servers_count; i++) {
        server_hash = hash_function_servers(&(main->hashring[i]));
        key_hash = hash_function_key(key);
        if (server_hash > key_hash){
            server = find_server(main, main->hashring[i], &index);
            *server_id = server->id;
            ok = 1;
            return server_retrieve(server, key);
        }
    }
    /* 
     * if the server was not already found,
     * then the correspondent server is the first one (index 0)
    */
    if (!ok) {
        server_memory *server = find_server(main, main->hashring[0], &index);
        *server_id = server->id;
        return server_retrieve(server, key);
    }
	return NULL;
}

void
loader_add_server(load_balancer* main, int server_id)
{
	server_memory *server = init_server_memory();
    server->id = server_id;
    /*
     * Creating the tags for the new server and
     * adding the 3 replicas to the hashring
    */
    for (int i = 0 ; i < 3; i++) {
        server->tags[i] = i * MAX_SERVERS + server->id;
        add_new_value(main->hashring, 3 * main->servers_count + i,
                    server->tags[i]);
    }
    main->servers[main->servers_count] = server;
    main->servers_count++;
    /* Distributing the products to the new server */
    if (main->servers_count > 1) {
        for (int i = 0; i < 3; i++) {
            distribute_products_add(main, server, i);
        }
    }
}

void
loader_remove_server(load_balancer* main, int server_id)
{
    int next_tag[3] = {0}, prev_tag[3] = {0}, index;
	server_memory * server = find_server(main, server_id, &index);
    remove_from_hashring(main, server, next_tag, prev_tag);
    for (int i = 0; i < 3; i++) {
        distribute_products_remove(main, server, prev_tag[i], next_tag[i]);
    }
    main->servers_count--;
    remove_from_server_arr(main, index);
    free_server_memory(server);
}

void
free_load_balancer(load_balancer* main)
{
    for (int i = 0 ; i < main->servers_count; i++) {
        free_server_memory(main->servers[i]);
    }
    free(main);
}
