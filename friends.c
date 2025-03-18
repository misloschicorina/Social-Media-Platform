#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"
#include "users.h"

graph_t *create_graph(int number_of_users)
{
	graph_t *graph = calloc(1, sizeof(*graph));
	DIE(!(graph), "CALLOC FAILED");
	graph->users_nr = number_of_users;
	graph->lists = calloc(number_of_users, sizeof(friends_list_t *));
	DIE(!(graph->lists), "CALLOC FAILED");
	for (int i = 0; i < number_of_users; i++) {
		graph->lists[i] = calloc(1, sizeof(friends_list_t));
		DIE(!(graph->lists[i]), "CALLOC FAILED");
	}
	return graph;
}

void add_friend_to_list(friends_list_t *friends_list, char *username)
{
	user_node_t *new_friend = calloc(1, sizeof(*new_friend));
	DIE(!(new_friend), "CALLOC FAILED");
	new_friend->id = get_user_id(username);
	new_friend->username = username;
	// Adaug mereu la inceputul listei de prieteni
	new_friend->next = friends_list->head;
	friends_list->head = new_friend;
	friends_list->size++;
}

void add_edge(graph_t *graph, char *username1, char *username2)
{
	int id1 = get_user_id(username1);
	int id2 = get_user_id(username2);

	add_friend_to_list(graph->lists[id1], username2);
	add_friend_to_list(graph->lists[id2], username1);
	printf("Added connection %s - %s\n", username1, username2);
}

void remove_friend_from_list(friends_list_t *friends_list, char *username)
{
	int id = get_user_id(username);
	user_node_t *curr = friends_list->head;
	user_node_t *prev = NULL;

	while (curr) {
		if (curr->id == id) {
			if (!prev)
				friends_list->head = curr->next;
			else
				prev->next = curr->next;
			break;
		}
		prev = curr;
		curr = curr->next;
	}
	if (curr)
		free(curr);
	friends_list->size--;
}

void remove_edge(graph_t *graph, char *username1, char *username2)
{
	int id1 = get_user_id(username1);
	int id2 = get_user_id(username2);
	remove_friend_from_list(graph->lists[id1], username2);
	remove_friend_from_list(graph->lists[id2], username1);
	printf("Removed connection %s - %s\n", username1, username2);
}

void calc_distance(graph_t *graph, char *username1, char *username2)
{
	int id1 = get_user_id(username1);
	int id2 = get_user_id(username2);

	bool *visited = calloc(graph->users_nr, sizeof(bool));
	int *distance = calloc(graph->users_nr, sizeof(int));
	int *queue = calloc(graph->users_nr, sizeof(int));

	DIE(!(visited), "CALLOC FAILED");
	DIE(!(distance), "CALLOC FAILED");
	DIE(!(queue), "CALLOC FAILED");

	int front = 0;
	int rear = 0;
	visited[id1] = true;
	queue[rear++] = id1;

	int found_dist = 0;
	for (int i = front; i < rear; i++) {
		int curr = queue[i];
		user_node_t *node = graph->lists[curr]->head;
		while (node) {
			if (!visited[node->id]) {
				visited[node->id] = true;
				distance[node->id] = distance[curr] + 1;
				queue[rear++] = node->id;

				if (node->id == id2) {
					found_dist = distance[node->id];
					break;
				}
			}
			node = node->next;
		}
		if (found_dist)
			break;
	}

	free(visited);
	free(queue);
	free(distance);

	if (found_dist)
		printf("The distance between %s - %s is %d\n",
			   username1, username2, found_dist);
	else
		printf("There is no way to get from %s to %s\n",
			   username1, username2);
}

bool is_friend(graph_t *graph, int id1, int id2)
{
	user_node_t *curr = graph->lists[id1]->head;
	while (curr) {
		if (curr->id == id2)
			return true;

		curr = curr->next;
	}
	return false;
}

void sort_array(int *arr, int size)
{
	for (int i = 0; i < size - 1; i++) {
		for (int j = i + 1; j < size; j++) {
			if (arr[i] > arr[j]) {
				int temp = arr[i];
				arr[i] = arr[j];
				arr[j] = temp;
			}
		}
	}
}

void suggestions(graph_t *graph, char *username)
{
	int found_suggestions = 0;
	int id = get_user_id(username);
	user_node_t *curr = graph->lists[id]->head;
	int *array = NULL; // vector in care voi tine id-urile prietenilor sugerati
	int array_len = 0;
	int array_capacity = 0;

	while (curr) { // traversez lista de prieteni a user-ului
		int friend_id = curr->id;
		user_node_t *it = graph->lists[friend_id]->head;

		while (it) { // Traversez lista fiecarui prieten al prietenului
			int friend_of_friend_id = it->id;

			if (friend_of_friend_id != id &&
				is_friend(graph, friend_of_friend_id, id) != true) {
				bool already_added = false;
				for (int j = 0; j < array_len; j++) {
					if (array[j] == friend_of_friend_id) {
						already_added = true;
						break;
					}
				}

				if (!already_added) {
					// Redimensionare vector daca e necesar
					if (array_len == array_capacity) {
						array_capacity =
						array_capacity == 0 ? 1 : array_capacity * 2;
						array = realloc(array, array_capacity * sizeof(*array));
					}

					// Adaug prietenul prietenului in vector
					array[array_len] = friend_of_friend_id;
					array_len++;
					found_suggestions++;
				}
			}

			it = it->next;
		}
		curr = curr->next;
	}
	sort_array(array, array_len);

	if (found_suggestions) {
		printf("Suggestions for %s:\n", username);
		for (int i = 0; i < array_len; i++) {
			char *name = get_user_name(array[i]);
			printf("%s\n", name);
		}
	} else {
		printf("There are no suggestions for %s\n", username);
	}
	free(array);
}

void common_friends(graph_t *graph, char *username1, char *username2)
{
	int found_common = 0;
	int id1 = get_user_id(username1);
	int id2 = get_user_id(username2);
	int *array = NULL;  // array ce stocheaza id-urile prietenilor comuni
	int array_len = 0;
	int array_capacity = 0;

	user_node_t *curr = graph->lists[id1]->head;
	while (curr) {
		if (array_len == array_capacity) {
			array_capacity = array_capacity == 0 ? 1 : array_capacity * 2;
			array = realloc(array, array_capacity * sizeof(*array));
		}
		if (is_friend(graph, curr->id, id2)) {
			array[array_len] = curr->id;
			array_len++;
			found_common++;
		}
		curr = curr->next;
	}

	sort_array(array, array_len);

	if (found_common) {
		printf("The common friends between %s and %s are:\n",
			   username1, username2);
		for (int i = 0; i < array_len; i++) {
			char *name = get_user_name(array[i]);
			printf("%s\n", name);
		}
	} else {
		printf("No common friends for %s and %s\n", username1, username2);
	}
	free(array);
}

void print_nr_friends(graph_t *graph, char *username)
{
	int id = get_user_id(username);
	printf("%s has %d friends\n", username, graph->lists[id]->size);
}

int nr_friends(graph_t *graph, int id)
{
	int nr = graph->lists[id]->size;
	return nr;
}

void most_popular(graph_t *graph, char *username)
{
	int id = get_user_id(username);
	// Consider initial ca user-ul e cel mai popular intre prietenii sai
	int popular_id = id;
	int max_friends_nr = nr_friends(graph, id);

	user_node_t *curr = graph->lists[id]->head;
	while (curr) {
		int nr = nr_friends(graph, curr->id);
		if (nr > max_friends_nr) { // am gasit un prieten mai popular
			max_friends_nr = nr;
			popular_id = curr->id;
		}
		// Verific daca am un prieten la fel de popular, dar cu id mai mic
		if (curr->id < popular_id && nr > id)
			popular_id = curr->id;
		curr = curr->next;
	}
	if (popular_id == id)
		printf("%s is the most popular\n",
			   get_user_name(popular_id));
	else
		printf("%s is the most popular friend of %s\n",
			   get_user_name(popular_id), username);
	free(curr);
}

void free_graph(graph_t *graph)
{
	for (int i = 0; i < graph->users_nr; i++) {
		user_node_t *curr = graph->lists[i]->head;
		while (curr) {
			user_node_t *temp = curr;
			curr = curr->next;
			free(temp);
		}
		free(graph->lists[i]);
	}
	free(graph->lists);
	free(graph);
}

void handle_input_friends(graph_t *graph, char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	char *username1;
	char *username2;

	if (!strcmp(cmd, "add")) {
		username1 = strtok(NULL, " ");
		username2 = strtok(NULL, "\n");
		add_edge(graph, username1, username2);
	} else if (!strcmp(cmd, "remove")) {
		username1 = strtok(NULL, " ");
		username2 = strtok(NULL, "\n");
		remove_edge(graph, username1, username2);
	} else if (!strcmp(cmd, "distance")) {
		username1 = strtok(NULL, " ");
		username2 = strtok(NULL, "\n");
		calc_distance(graph, username1, username2);
	} else if (!strcmp(cmd, "suggestions")) {
		username1 = strtok(NULL, "\n");
		suggestions(graph, username1);
	} else if (!strcmp(cmd, "common")) {
		username1 = strtok(NULL, " ");
		username2 = strtok(NULL, "\n");
		common_friends(graph, username1, username2);
	} else if (!strcmp(cmd, "friends")) {
		username1 = strtok(NULL, "\n");
		print_nr_friends(graph, username1);
	} else if (!strcmp(cmd, "popular")) {
		username1 = strtok(NULL, "\n");
		most_popular(graph, username1);
	}

	free(commands);
}
