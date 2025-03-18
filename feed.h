#ifndef FEED_H
#define FEED_H

#include <stdbool.h>
#include "friends.h"
#include "posts.h"

//  Function that handles the calling of every command from task 3
void handle_input_feed(linked_list_t *all_posts, graph_t *graph, char *input);

// Declaratiile functiilor din feed.c
void show_feed(linked_list_t *all_posts, graph_t *graph,
			   char *username, int nr_posts);
g_node_t *traverse_repost_tree(g_node_t *node, int repost_id);
const char *get_post_title_from_repost(int repost_id, linked_list_t *all_posts);
void view_profile(linked_list_t *all_posts, char *username);
void show_friends_that_reposted(linked_list_t *all_posts,
								graph_t *graph, char *username, int post_id);
bool is_clique(int *clique, int size, graph_t *graph);
void find_largest_clique(graph_t *graph, int user_id, int *clique,
						 int *clique_size);
void show_group(graph_t *graph, char *username);

#endif // FEED_H
