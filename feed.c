#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"
#include "friends.h"
#include "posts.h"
#include "users.h"

void show_feed(linked_list_t *all_posts, graph_t *graph, char *username,
			   int nr_posts)
{
	int id = get_user_id(username);
	ll_node_t *curr = all_posts->tail;
	while (curr && nr_posts) {
		if (is_friend(graph, curr->data->user_id, id) ||
		    curr->data->user_id == id) {
			if (curr->data->is_repost == 0) { // sar repostarile din lista
				printf("%s: %s\n", get_user_name(curr->data->user_id),
					   curr->data->title_post);
				nr_posts--;
			}
		}
		curr = curr->prev;
	}
}

g_node_t *traverse_repost_tree(g_node_t *node, int repost_id)
{
	if (!node)
		return NULL;

	posts_t *post = (posts_t *)node->value;
	if (post->post_id == repost_id)
		return node;

	for (int i = 0; i < node->n_children; i++) {
		g_node_t *repost_node = traverse_repost_tree(node->children[i],
													 repost_id);
		if (repost_node)
			return repost_node;
	}

	return NULL; // Return NULL if the repost_id was not found
}

const char *get_post_title_from_repost(int repost_id, linked_list_t *all_posts)
{
	ll_node_t *curr = all_posts->head;

	while (curr) {
		g_node_t *root = curr->init_root->root;
		g_node_t *repost_node = NULL;

		// Traverse the repost tree manually to find the repost node
		if (root)
			repost_node = traverse_repost_tree(root, repost_id);

		if (repost_node) {
			// If the repost is found, return the title of the original post
			posts_t *original_post = (posts_t *)root->value;
			return original_post->title_post;
		}
		curr = curr->next;
	}

	return NULL; // Return NULL if the repost_id was not found
}

void view_profile(linked_list_t *all_posts, char *username)
{
	int id = get_user_id(username);
	const char *title;
	ll_node_t *curr = all_posts->head;
	while (curr) {
		if ((int)curr->data->user_id == id) {
			if (curr->data->is_repost == 0)
				printf("Posted: %s\n", curr->data->title_post);
			if (curr->data->is_repost == 1) {
				title =
				get_post_title_from_repost(curr->data->post_id, all_posts);
				printf("Reposted: %s\n", title);
			}
		}
		curr = curr->next;
	}
}

void show_friends_that_reposted(linked_list_t *all_posts, graph_t *graph,
								char *username, int post_id)
{
	ll_node_t *curr = all_posts->head;
	int user_id = get_user_id(username);
	while (curr) {
		if (curr->data->original_post_id == post_id) {
			if (is_friend(graph, curr->data->user_id, user_id))
				printf("%s\n", get_user_name(curr->data->user_id));
		}
		curr = curr->next;
	}
}

bool is_clique(int *clique, int size, graph_t *graph)
{
	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++) {
			if (!is_friend(graph, clique[i], clique[j]))
				return false;
		}
	}
	return true;
}

void find_largest_clique(graph_t *graph, int user_id,
						 int *clique, int *clique_size)
{
	int *friends = calloc(graph->users_nr, sizeof(int));
	DIE(!(friends), "CALLOC FAILED");
	int friends_size = 0;

	user_node_t *curr = graph->lists[user_id]->head;
	while (curr) {
		friends[friends_size++] = curr->id;
		curr = curr->next;
	}

	int *temp_clique = calloc((friends_size + 1), sizeof(int));
	DIE(!(temp_clique), "CALLOC FAILED");
	temp_clique[0] = user_id;
	int max_clique_size = 1;

	for (int i = 1; i < (1 << friends_size); i++) {
		int temp_size = 1;
		for (int j = 0; j < friends_size; j++) {
			if (i & (1 << j))
				temp_clique[temp_size++] =
				friends[j];
		}
		if (is_clique(temp_clique, temp_size, graph) &&
			temp_size > max_clique_size) {
			max_clique_size = temp_size;
			memcpy(clique, temp_clique, temp_size * sizeof(int));
		}
	}

	*clique_size = max_clique_size;

	free(friends);
	free(temp_clique);
}

void show_group(graph_t *graph, char *username)
{
	int user_id = get_user_id(username);
	int clique_size = 0;
	int *clique = calloc(graph->users_nr, sizeof(int));
	DIE(!(clique), "CALLOC FAILED");

	find_largest_clique(graph, user_id, clique, &clique_size);

	if (clique_size > 1) {
		sort_array(clique, clique_size);
		printf("The closest friend group of %s is:\n", username);
		for (int i = 0; i < clique_size; i++)
			printf("%s\n", get_user_name(clique[i]));
	} else {
		printf("%s\n", username);
	}

	free(clique);
}

void handle_input_feed(linked_list_t *all_posts, graph_t *graph, char *input)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	char *username;
	const char *post_id_str;
	int nr, post_id;

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed")) {
		username = strtok(NULL, " ");
		nr = atoi(strtok(NULL, "\n"));
		show_feed(all_posts, graph, username, nr);
	} else if (!strcmp(cmd, "view-profile")) {
		username = strtok(NULL, "\n");
		view_profile(all_posts, username);
	} else if (!strcmp(cmd, "friends-repost")) {
		username = strtok(NULL, " ");
		post_id_str = strtok(NULL, "\n");
		post_id = atoi(post_id_str);
		show_friends_that_reposted(all_posts, graph, username, post_id);
	} else if (!strcmp(cmd, "common-group")) {
		username = strtok(NULL, "\n");
		show_group(graph, username);
	}

	free(commands);
}
