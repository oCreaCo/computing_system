#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define NUM_INCREMENT	(10000000)

uint64_t global_count = 0;

void increase_global_count(void)
{
	uint64_t old_value = global_count;
	uint64_t new_value = old_value + 1;
	global_count = new_value;

	/**
	 * @details
	 * This function could be replaced with a single line of code:
	 * - global_count++;
	 * - global_count += 1;
	 * 
	 * However, these codes execute three instructions internally:
	 * 1. load global_count (memory -> CPU)
	 * 2. calculate new value
	 * 3. store global_count (CPU -> memory)
	 * 
	 * The CPU executes code instruction by instruction, so writing a single
	 * code line will eventually produce the same result.
	 */
}

void *thread_func(void *arg)
{
	uint64_t local_count = 0;

	for (int i = 0; i < NUM_INCREMENT; i++) {
		increase_global_count();	// increase global count
		local_count += 1;			// increase local count
	}

	return (void *)local_count;
}

int main(int argc, char *argv[])
{
	int num_threads;
	pthread_t *threads;
	uint64_t ret;

	if (argc != 2) {
		printf("error: invalid argument, usage: ./pthread_test (num_threads)\n");
		return 1;
	}

	num_threads = atoi(argv[1]);
	threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);

	//create threads
	for (int i = 0; i < num_threads; i++) {
		if (pthread_create(&threads[i], NULL, thread_func, NULL) < 0) {
			printf("error: pthread_create failed!\n");
			return 1;
		}
	}

	// wait the threads end
	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], (void **)&ret);
		printf("thread %ld: local count -> %ld\n", threads[i], ret);
	}

	printf("global count -> %ld\n", global_count);

	free(threads);

	return 0;
}