#include <stdbool.h>
#ifndef FRIENDS_H
#define FRIENDS_H

#define MAX_COMMAND_LEN 500
#define MAX_PEOPLE 550

#define DIE(assertion, call_description)            \
	do                                              \
	{                                               \
		if (assertion)                              \
		{                                           \
			fprintf(stderr, "(%s, %d): ", __FILE__, \
					__LINE__);                      \
			perror(call_description);               \
			exit(errno);                            \
		}                                           \
	} while (0)

typedef struct user_node {
	int id;
	char *username;
	struct user_node *next;
} user_node_t;

typedef struct {
	user_node_t *head;
	int size;
} friends_list_t;

typedef struct
{
	friends_list_t **lists; /* Listele de adiacenta ale grafului */
	int users_nr; /* Numarul de noduri din graf. */
} graph_t;

// Function that handles the calling of every command from task 1
void handle_input_friends(graph_t *graph, char *input);

// Declaratiile functiilor din friends.c
graph_t *create_graph(int number_of_users);
void add_friend_to_list(friends_list_t *friends_list, char *username);
void add_edge(graph_t *graph, char *username1, char *username2);
void remove_friend_from_list(friends_list_t *friends_list, char *username);
void remove_edge(graph_t *graph, char *username1, char *username2);
void calc_distance(graph_t *graph, char *username1, char *username2);
bool is_friend(graph_t *graph, int id1, int id2);
void sort_array(int *arr, int size);
void suggestions(graph_t *graph, char *username);
void common_friends(graph_t *graph, char *username1, char *username2);
void print_nr_friends(graph_t *graph, char *username);
int nr_friends(graph_t *graph, int id);
void most_popular(graph_t *graph, char *username);
void free_graph(graph_t *graph);

#endif // FRIENDS_H
