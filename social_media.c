/**
 * The entrypoint of the homework. Every initialization must be done here
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"

/**
 * Initializez every task based on which task we are running
*/
void init_tasks(graph_t **graph, linked_list_t **all_posts,
				int *post_id_counter)
{
	#ifdef TASK_1
		*graph = create_graph(MAX_PEOPLE);
	#else
		(void)graph;
	#endif

	#ifdef TASK_2
		*all_posts = create_list();
		*post_id_counter = 0;
	#else
		(void)all_posts;
		(void)post_id_counter;
	#endif

	#ifdef TASK_3

	#endif
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	graph_t *graph;
	linked_list_t *all_posts;
	int post_id_counter = 0;
	init_tasks(&graph, &all_posts, &post_id_counter);

	char *input = (char *)malloc(MAX_COMMAND_LEN);
	while (1) {
		char *command = fgets(input, MAX_COMMAND_LEN, stdin);

		// If fgets returns null, we reached EOF
		if (!command)
			break;

		#ifdef TASK_1
		handle_input_friends(graph, input);
		#endif

		#ifdef TASK_2
		handle_input_posts(input, all_posts, &post_id_counter);
		#endif

		#ifdef TASK_3
		handle_input_feed(all_posts, graph, input);
		#endif
	}

	free_users();
	free(input);

	#ifdef TASK_1
	free_graph(graph);
	#endif

	#ifdef TASK_2
	free_list(all_posts);
	#endif

	return 0;
}
