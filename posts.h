#ifndef POSTS_H
#define POSTS_H
#define POST_LENGHTH 281

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

typedef struct g_node_t {
	void *value;
	struct g_node_t **children; // repostarile
	int n_children;
} g_node_t;

typedef struct g_tree_t {
	g_node_t *root;
} g_tree_t;

typedef struct posts_t {
	int post_id;
	char title_post[POST_LENGHTH];
	int user_id;
	g_node_t **repost; // reposturile de la postare
	int likes;
	int *users_like; // userii care au dat like
	int is_repost;
	int original_post_id;
} posts_t;

typedef struct ll_node_t {
	posts_t *data;
	struct ll_node_t *next;
	struct ll_node_t *prev;
	g_tree_t *init_root;
} ll_node_t;

typedef struct linked_list_t {
	ll_node_t *head;
	unsigned int size;
	ll_node_t *tail;
} linked_list_t; // lista de arbori

// Function that handles the calling of every command from task 2
void handle_input_posts(char *input, linked_list_t *all_posts,
						int *post_id_counter);

// Declaratiile functiilor din posts.c
g_node_t *create_node(void *value, int n_children);
linked_list_t *create_list(void);
g_tree_t *create_tree(posts_t *new_data);
void add_node(linked_list_t *list, posts_t *new_data);
posts_t *create_post(char *username, char *title,
					 linked_list_t *all_posts, int *post_id_counter);
void add_child_to_node(g_node_t *parent_node, posts_t *new_post);
g_node_t *find_repost(g_node_t *node, int repost_id);
void free_node_recursive(g_node_t *node);
void create_repost(char *username, int post_id, int repost_id,
				   linked_list_t *all_posts, int *post_id_counter);
void print_reposts(g_node_t *node);
void get_reposts(int post_id, int repost_id, linked_list_t *all_posts);
void update_likes(posts_t *target_post, int user_id, char *username,
				  char *title_post, int repost_id);
void handle_likes(int post_id, int repost_id, char *username,
				  linked_list_t *all_posts);
void get_likes(int post_id, int repost_id, linked_list_t *all_posts);
g_node_t *find_max_likes(g_node_t *node, g_node_t *max_node);
void ratio(linked_list_t *all_posts, int post_id);
void free_list(linked_list_t *list);
void delete_tree(g_node_t *node);
void remove_child(g_node_t *parent, g_node_t *child);
void delete_post_from_list(linked_list_t *all_posts, ll_node_t *node_to_delete);
void delete_repost(g_node_t *root, int repost_id, char *post_title);
void delete_post(linked_list_t *all_posts, int post_id, int repost_id);
g_node_t *lca(g_node_t *current_node, g_node_t *node1, g_node_t *node2);
void find_common_repost(int post_id, int repost_id1, int repost_id2,
						linked_list_t *all_posts);

#endif // POSTS_H
