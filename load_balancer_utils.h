/* Copyright 2021 Raduta Lavinia-Maria 313CA */

#ifndef LOAD_BALANCER_UTILS_H_
#define LOAD_BALANCER_UTILS_H_

#include "server.h"
#include "load_balancer.h"

#define MAX_SERVERS 100000

unsigned int
hash_function_servers(void *a);

unsigned int
hash_function_key(void *a);

/**
 * add_new_value() - Adds the replicas to the hashring,
 *                keeping the order of the hashes.
 * @arg1: The array in which the new value is added (hashring).
 * @arg2: The size of the array before the add.
 * @arg3: Value that has to be added.
 */
void
add_new_value(int *v, int size, int value);

/**
 * find_server() - Returns the server that has a specific tag,
 *                 also keeping its index in the servers array
 * @arg1: Load balancer which distributes the work.
 * @arg2: The tag in question.
 * @arg3: The index of the replica with the tag in the hashring.
 */
server_memory*
find_server(load_balancer *main, int tag, int *index);

/**
 * find_keys() - Function used in the add_server function
 *             - Used to identify the products that need to be replaced
 *               and replace them in the correct server.
 * @arg1: The freshly added server.
 * @arg2: The server whose replica is the next for the current server.
 * @arg3: The hash of the next server in hashring.
 * @arg4: The hash of the previous server in hashring.
 * @arg5: The index of the new server's replica in the hashring.
 */
void
find_keys(server_memory* server, server_memory* next_server,
    unsigned int next_hash, unsigned int prev_hash, int i);

/**
 * find_neighbor_indexes()
 *       - Function used in the add_server function
 *       - Used to identify the indexes of the next and previous replicas
 *         on the hashring, that are not of the same server as the current one
 * @arg1: Load balancer which distributes the work.
 * @arg2: The freshly added server.
 * @arg3: The index of one of the replicas of the new server on the hashring.
 * @arg4: The position of the next server in hashring.
 * @arg5: The position of the previous server in hashring.
 */
void
find_neighbor_indexes(load_balancer* main, server_memory* server,
    int j, int *prev_index, int *next_index);


/**
 * distribute_products_add()
 *       - The function coordinates the distribution of products for the
 *         freshly added server.
 * @arg1: Load balancer which distributes the work.
 * @arg2: The freshly added server.
 * @arg3: Which tag of the server is currently observed (1, 2 or 3).
 */
void
distribute_products_add(load_balancer* main,
    server_memory* server, int i);


/**
 * find_neighbor_tags_remove()
 *       - Function used in the remove_server function
 *       - Used to identify the tags of the next and previous replicas
 *         on the hashring, that are not of the same server as the current one
 * @arg1: Load balancer which distributes the work.
 * @arg2: The server that will be removed.
 * @arg3: Array of posision of servers's replicas on the hashring.
 * @arg4: The tag of the next server in hashring.
 * @arg5: The tag of the previous server in hashring.
 */
void
find_neighbor_tags_remove(load_balancer* main, server_memory* server,
    int* pos, int* next_tag, int* prev_tag);

/**
 * remove_from_hashring()
 *       - Function used in the remove_server function
 *       - Used to delete all the server replicas from the hashring and
 *         also keeping the neighbors (that are not the same server) index
 * @arg1: Load balancer which distributes the work.
 * @arg2: The server that is removed.
 * @arg3: The tag of the next server replica in hashring.
 * @arg4: The tag of the previous server replica in hashring.
 */
void
remove_from_hashring(load_balancer* main, server_memory *server,
    int *next_tag, int *prev_tag);

/**
 * remove_from_server_arr()
 *       - Removes the server at the index given as parameter from
 *         the servers array.
 * @arg1: Load balancer which distributes the work.
 * @arg2: The index of the server to be removed.
 */
void
remove_from_server_arr(load_balancer *main, int index);

/**
 * distribute_products_remove()
 *      - Function used in the remove_server function.
 *      - Distributes the products of the neighbor servers of the one that is
 *        removed, keeping the hashring order.
 * @arg1: Load balancer which distributes the work.
 * @arg2: The server that will be removed.
 * @arg3: The next replica tag on the hashring.
 * @arg4: The previous replica tag on the hashring.
 */
void
distribute_products_remove(load_balancer* main, server_memory* server,
    int prev, int next);

#endif  /* LOAD_BALANCER_UTILS_H_ */
