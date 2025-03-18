#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posts.h"
#include "users.h"

#define POST_LENGHTH 281

g_node_t *create_node(void *value, int n_children)
{
	g_node_t *node = calloc(1, sizeof(g_node_t));
	DIE(!(node), "CALLOC FAILED");
	node->value = value;
	node->n_children = n_children;

	if (n_children > 0) {
		node->children = calloc(n_children, sizeof(g_node_t *));
		DIE(!(node->children), "CALLOC FAILED");
	} else {
		node->children = NULL;
	}

	return node;
}

linked_list_t *create_list(void)
{
	linked_list_t *list = calloc(1, sizeof(linked_list_t));
	DIE(!list, "CALLOC FAILED");
	return list;
}

g_tree_t *create_tree(posts_t *new_data)
{
	g_tree_t *tree = (g_tree_t *)calloc(1, sizeof(g_tree_t));
	DIE(!tree, "CALLOC FAILED");
	tree->root = create_node(new_data, 0);
	return tree;
}

void add_node(linked_list_t *list, posts_t *new_data)
{
	ll_node_t *new_node = calloc(1, sizeof(*new_node));
	DIE(!new_node, "CALLOC FAILED");
	new_node->data = new_data;
	new_node->next = NULL;
	new_node->prev = list->tail;
	new_node->init_root = create_tree(new_data);

	if (!list->head) {
		list->head = new_node;
		list->tail = new_node;
	} else {
		list->tail->next = new_node;
		list->tail = new_node;
	}
	list->size++;
}

posts_t *create_post(char *username, char *title, linked_list_t *all_posts,
					 int *post_id_counter)
{
	posts_t *new_post = (posts_t *)calloc(1, sizeof(posts_t));
	DIE(!new_post, "CALLOC FAILED");

	(*post_id_counter)++;
	new_post->post_id = *post_id_counter;

	if (title) {
		strncpy(new_post->title_post, title, POST_LENGHTH - 1);
		new_post->title_post[POST_LENGHTH - 1] = '\0';
	}

	new_post->user_id = get_user_id(username);

	// Adauga postarea la lista de postari
	add_node(all_posts, new_post);

	return new_post;
}

void add_child_to_node(g_node_t *parent_node, posts_t *new_post)
{
	// Creeaza un nou nod pentru postare
	g_node_t *new_node = create_node(new_post, 0);

	// Realocarea memoriei pentru a adauga noul nod ca fiu al nodului dat
	parent_node->children =
	realloc(parent_node->children,
			(parent_node->n_children + 1)  * sizeof(g_node_t *));
	// Mereu se pune noul nod la dreapta
	parent_node->children[parent_node->n_children] = new_node;
	parent_node->n_children++;
}

g_node_t *find_repost(g_node_t *node, int repost_id)
{
	if (!node)
		return NULL;

	posts_t *post = (posts_t *)node->value;
	if (post->post_id == repost_id)
		return node;

	// Parcurge recursiv copiii nodului curent
	for (int i = 0; i < node->n_children; i++) {
		g_node_t *repost_node = find_repost(node->children[i], repost_id);
		if (repost_node)
			return repost_node;
	}

	// Postarea de repostare nu a fost gasita in subarbore
	return NULL;
}

void free_node_recursive(g_node_t *node)
{
	if (!node)
		return;

	for (int i = 0; i < node->n_children; i++)
		free_node_recursive(node->children[i]);

	// Eliberarea memoriei pt array-ul de children si pt nod
	free(node->children);
	free(node);
}

void create_repost(char *username, int post_id,
				   int repost_id, linked_list_t *all_posts,
				   int *post_id_counter)
{
	ll_node_t *curr = all_posts->head;
	while (curr) {
		posts_t *post = curr->data;
		if (post->post_id == post_id) {
			// Creez postarea de repostare
			posts_t *repost = create_post(username, NULL,
				all_posts, post_id_counter);
			repost->is_repost = 1;
			repost->original_post_id = post_id;

			int num_reposts = curr->init_root->root->n_children;
			// Daca repost_id este 0, adaug repostarea la postarea originala
			if (!repost_id) {
				add_child_to_node(curr->init_root->root, repost);
			} else {
				// Caut repostarea in lista de repostari existente
				for (int i = 0; i < num_reposts; i++) {
					g_node_t *found_node =
					curr->init_root->root->children[i];
					g_node_t *repost_node = find_repost(found_node, repost_id);
					if (repost_node) {
						// Adaug postarea ca si copil al postarii
						add_child_to_node(repost_node, repost);
						break;
					}
				}
			}
			printf("Created repost #%d for %s\n", *post_id_counter, username);
			return;
		}
		curr = curr->next;
	}
}

void print_reposts(g_node_t *node)
{
	if (!node)
		return;

	posts_t *post = (posts_t *)node->value;
	printf("Repost #%d by %s\n", post->post_id, get_user_name(post->user_id));

	for (int i = 0; i < node->n_children; i++)
		print_reposts(node->children[i]);
}

void get_reposts(int post_id, int repost_id, linked_list_t *all_posts)
{
	ll_node_t *curr = all_posts->head;
	while (curr) {
		posts_t *post = curr->data;
		if (post->post_id == post_id) {
			int num_reposts = curr->init_root->root->n_children;
			if (repost_id == 0) {
				// Postare originala de la radacina arborelui
				printf("%s - Post by %s\n", post->title_post,
					   get_user_name(post->user_id));
				for (int i = 0; i < num_reposts; i++)
					print_reposts(curr->init_root->root->children[i]);
			} else {
				for (int i = 0; i < num_reposts; i++) {
					g_node_t *searched_tree =
					curr->init_root->root->children[i];
					g_node_t *repost_node = find_repost(searched_tree,
						repost_id);
					if (repost_node) {
						// Am gasit repostarea
						print_reposts(repost_node);
						return;
					}
				}
			}
			return;
		}
		curr = curr->next;
	}
}

void update_likes(posts_t *target_post, int user_id, char *username,
				  char *title_post, int repost_id)
{
	int liked = 0; // presupunem ca postarea nu e liked
	for (int i = 0; i < target_post->likes; i++) {
		if (target_post->users_like[i] == user_id) {
			liked = 1; // trebuie sa-i dam unlike
			for (int j = i; j < target_post->likes - 1; j++)
				target_post->users_like[j] = target_post->users_like[j + 1];

			target_post->likes--;
			if (target_post->likes > 0) {
				target_post->users_like =
				realloc(target_post->users_like,
						target_post->likes  * sizeof(int));
			} else {
				free(target_post->users_like);
				target_post->users_like = NULL;
			}
			if (repost_id)
				printf("User %s unliked repost %s\n",
					   username, title_post);
			else
				printf("User %s unliked post %s\n",
					   username, target_post->title_post);
			break;
		}
	}
	if (!liked) {
		target_post->likes++;
		target_post->users_like =
		realloc(target_post->users_like, target_post->likes  * sizeof(int));
		target_post->users_like[target_post->likes - 1] = user_id;
		if (repost_id)
			printf("User %s liked repost %s\n",
				   username, title_post);
		else
			printf("User %s liked post %s\n",
				   username, target_post->title_post);
	}
}

void handle_likes(int post_id, int repost_id,
				  char *username, linked_list_t *all_posts)
{
	int user_id = get_user_id(username);
	ll_node_t *curr = all_posts->head;
	g_node_t *target_node = NULL; // nodul la care dam like
	char *title_post;

	while (curr) {
		posts_t *post = curr->data;

		if (post_id == post->post_id) {
			title_post = strdup(post->title_post); // titlul postarii originale
			if (!repost_id) {
				target_node = curr->init_root->root;
			} else { // e de la o repostare, cautam in arbore
				int num_reposts = curr->init_root->root->n_children;
				for (int i = 0; i < num_reposts; i++) {
					g_node_t *sub_tree =
					curr->init_root->root->children[i];
					g_node_t *found_node = find_repost(sub_tree, repost_id);
					if (found_node) {
						target_node = found_node;
						break;
					}
				}
			}
			if (target_node) {
				posts_t *target_post = (posts_t *)target_node->value;
				update_likes(target_post, user_id, username,
							 title_post, repost_id);
				break;
			}
		}
		curr = curr->next;
	}
	if (title_post)
		free(title_post);
}

void get_likes(int post_id, int repost_id, linked_list_t *all_posts)
{
	ll_node_t *curr = all_posts->head;

	while (curr) {
		posts_t *post = curr->data;

		if (post->post_id == post_id) {
			if (!repost_id) {
				printf("Post %s has %d likes\n",
					   post->title_post, post->likes);
			} else {
				int num_reposts =
				curr->init_root->root->n_children;

				for (int i = 0; i < num_reposts; i++) {
					g_node_t *found_post =
					find_repost(curr->init_root->root->children[i],
								repost_id);
					if (found_post) {
						printf("Repost #%d has %d likes\n",
							   ((posts_t *)found_post->value)->post_id,
							   ((posts_t *)found_post->value)->likes);
						break;
					}
				}
			}
			return;
		}
		curr = curr->next;
	}
}

g_node_t *find_max_likes(g_node_t *node, g_node_t *max_node)
{
	if (!node)
		return max_node;

	posts_t *post = (posts_t *)node->value;
	posts_t *max_post = (posts_t *)(max_node ? max_node->value : NULL);

	if (!max_node || post->likes > max_post->likes)
		max_node = node;

	for (int i = 0; i < node->n_children; i++)
		max_node = find_max_likes(node->children[i], max_node);

	return max_node;
}

void ratio(linked_list_t *all_posts, int post_id)
{
	ll_node_t *curr_node = all_posts->head;
	g_node_t *root;
	while (curr_node) {
		if (curr_node->data->post_id == post_id) {
			root = curr_node->init_root->root;
			g_node_t *max_like_node = find_max_likes(root, NULL);
			if (max_like_node) {
				posts_t *max_like_post = (posts_t *)max_like_node->value;
				if (((posts_t *)root->value)->likes < max_like_post->likes)
					printf("Post %d got ratio'd by repost %d\n",
						   ((posts_t *)root->value)->post_id,
						   max_like_post->post_id);
				else
					printf("The original post is the highest rated\n");
			}
			return;
		}
		curr_node = curr_node->next;
	}
}

void free_list(linked_list_t *list)
{
	ll_node_t *curr = list->head;

	while (curr) {
		ll_node_t *next_node = curr->next;

		posts_t *post = (posts_t *)curr->data;
		if (post) {
			free(post->repost);
			free(post->users_like);
			free(post);
		}

		if (curr->init_root) {
			free_node_recursive(curr->init_root->root);
			free(curr->init_root);
		}

		free(curr);
		curr = next_node;
	}

	free(list);
}

void delete_tree(g_node_t *node)
{
	if (!node)
		return;

	for (int i = 0; i < node->n_children; i++)
		delete_tree(node->children[i]);

	posts_t *post = (posts_t *)node->value;

	if (post)
		free(post->repost);

	free(node->children);
	free(node);
}

void remove_child(g_node_t *parent, g_node_t *child)
{
	int index = -1;
	for (int i = 0; i < parent->n_children; i++) {
		if (parent->children[i] == child) {
			index = i;
			break;
		}
	}
	if (index == -1)
		return;

	for (int i = index; i < parent->n_children - 1; i++)
		parent->children[i] = parent->children[i + 1];

	parent->n_children--;
	parent->children =
	realloc(parent->children, parent->n_children  * sizeof(g_node_t *));
}

void delete_post_from_list(linked_list_t *all_posts, ll_node_t *node_to_delete)
{
	if (!all_posts || !node_to_delete)
		return;

	ll_node_t *current = all_posts->head;

	while (current) {
		if (current == node_to_delete) {
			if (current->prev)
				current->prev->next = current->next;
			else
				all_posts->head = current->next;

			if (current->next)
				current->next->prev = current->prev;
			else
				all_posts->tail = current->prev;

			posts_t *post = current->data;

			free(post->repost);
			free(post->users_like);
			free(post);

			free_node_recursive(current->init_root->root);
			free(current->init_root);
			free(current->data);

			free(current);
			all_posts->size--;
			return;
		}
		current = current->next;
	}
}

void delete_repost(g_node_t *root, int repost_id, char *post_title)
{
	g_node_t *target_node = find_repost(root, repost_id);

	if (target_node) {
		posts_t *target_post = (posts_t *)target_node->value;
		printf("Deleted repost #%d of post %s\n",
			   target_post->post_id, post_title);

		for (int i = 0; i < root->n_children; i++) {
			if (root->children[i] == target_node) {
				remove_child(root, target_node);
				delete_tree(target_node);
				break;
			}
		}
	}
}

void delete_post(linked_list_t *all_posts, int post_id, int repost_id)
{
	ll_node_t *curr = all_posts->head;

	while (curr) {
		posts_t *post = curr->data;

		if (post_id == post->post_id) {
			if (!repost_id) {
				// Stergere postare si arbore de repostari
				printf("Deleted %s\n", post->title_post);
				delete_post_from_list(all_posts, curr);
			} else {
				// Caut repostarea in arbore
				g_node_t *root = curr->init_root->root;
				delete_repost(root, repost_id, post->title_post);
			}
			return;
		}
		curr = curr->next;
	}
}

g_node_t *lca(g_node_t *curr, g_node_t *node1, g_node_t *node2)
{
	if (!curr)
		return NULL;

	if (curr == node1 || curr == node2)
		return curr;

	// Caut LCA in subarborii fiecarui nod copil
	g_node_t *found_lca = NULL;
	for (int i = 0; i < curr->n_children; i++) {
		g_node_t *child_lca = lca(curr->children[i], node1, node2);
		if (child_lca) {
			if (!found_lca) {
				found_lca = child_lca;
			} else {
				// am gasit deja un LCA în alt subarbore, atunci curr este LCA
				return curr;
			}
		}
	}

	return found_lca;
}

void find_common_repost(int post_id, int repost_id1,
						int repost_id2, linked_list_t *all_posts)
{
	g_node_t *repost1 = NULL;
	g_node_t *repost2 = NULL;
	g_node_t *root = NULL;

	// Caut fiecare repost in lista
	ll_node_t *curr = all_posts->head;
	while (curr) {
		posts_t *post = curr->data;
		if (post->post_id == post_id) {
			root = curr->init_root->root;
			int num_repost = curr->init_root->root->n_children;
			for (int i = 0; i < num_repost; i++) {
				// Caut repost_id1 si repost_id2 in arborele de reposturi
				g_node_t *searched_tree = curr->init_root->root->children[i];
				g_node_t *found_node = find_repost(searched_tree, repost_id1);

				if (found_node)
					repost1 = found_node;

				found_node = find_repost(searched_tree, repost_id2);
				if (found_node)
					repost2 = found_node;

				if (repost1 && repost2) // am gasit ambele reposturi
					break;
			}
			if (repost1 && repost2) // am gasit ambele reposturi
				break;
		}
		curr = curr->next;
	}

	if (repost1 && repost2) {
		g_node_t *common_repost = lca(root, repost1, repost2);

		if (common_repost) {
			int post_id = ((posts_t *)(common_repost->value))->post_id;
			printf("The first common repost of %d and %d is %d\n",
				   repost_id1, repost_id2, post_id);
		}
	}
}

void handle_input_posts(char *input, linked_list_t *all_posts,
						int *post_id_counter)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");
	int post_id, repost_id = 0, repost_id1, repost_id2;
	char *username, *title, *repost_id_str;

	if (!cmd)
		return;

	if (!strcmp(cmd, "create")) {
		username = strtok(NULL, " ");
		title = strtok(NULL, "\n");
		create_post(username, title, all_posts, post_id_counter);
		printf("Created %s for %s\n", title, username);

	} else if (!strcmp(cmd, "repost")) {
		username = strtok(NULL, " ");
		post_id = atoi(strtok(NULL, " "));
		repost_id_str = strtok(NULL, "\n");

		if (repost_id_str)
			repost_id = atoi(repost_id_str);
		create_repost(username, post_id, repost_id, all_posts, post_id_counter);

	} else if (!strcmp(cmd, "common-repost")) {
		post_id = atoi(strtok(NULL, " "));
		repost_id1 = atoi(strtok(NULL, " "));
		repost_id2 = atoi(strtok(NULL, "\n"));
		find_common_repost(post_id, repost_id1, repost_id2, all_posts);

	} else if (!strcmp(cmd, "like")) {
		username = strtok(NULL, " ");
		post_id = atoi(strtok(NULL, " "));
		repost_id_str = strtok(NULL, "\n");

		if (repost_id_str)
			repost_id = atoi(repost_id_str);
		handle_likes(post_id, repost_id, username, all_posts);

	} else if (!strcmp(cmd, "ratio")) {
		post_id = atoi(strtok(NULL, "\n"));
		ratio(all_posts, post_id);

	} else if (!strcmp(cmd, "delete")) {
		post_id = atoi(strtok(NULL, " "));
		repost_id_str = strtok(NULL, "\n");

		if (repost_id_str)
			repost_id = atoi(repost_id_str);
		delete_post(all_posts, post_id, repost_id);

	} else if (!strcmp(cmd, "get-likes")) {
		post_id = atoi(strtok(NULL, " "));
		repost_id_str = strtok(NULL, "\n");

		if (repost_id_str)
			repost_id = atoi(repost_id_str);
		get_likes(post_id, repost_id, all_posts);

	} else if (!strcmp(cmd, "get-reposts")) {
		post_id = atoi(strtok(NULL, " "));
		repost_id_str = strtok(NULL, "\n");

		if (repost_id_str)
			repost_id = atoi(repost_id_str);
		get_reposts(post_id, repost_id, all_posts);
	}
	free(commands);
}
