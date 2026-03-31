#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#define NUM_READERS 3
#define NUM_WRITERS 1

static int shared_data = 0;
static pthread_rwlock_t rw_lock;

static volatile int keep_running  = 1;

void signal_handler(int sig)
{
    keep_running = 0;
}

void *reader_thread(void *arg)
{
    int id =  *(int *)arg;

    while(keep_running) {
        pthread_rwlock_rdlock(&rw_lock);
        printf("Reader %d, value = %d\n", id, shared_data);

        usleep(1000000);

        pthread_rwlock_unlock(&rw_lock);

        usleep(500000);
    }

    free(arg);
    return NULL;
}

void *writer_thread(void *arg)
{
    int id = *(int *)arg;
    static int writer_counter = 0;

    while(keep_running) {
        pthread_rwlock_wrlock(&rw_lock);

        writer_counter++;
        shared_data = writer_counter;
        printf("Writer %d: New value = %d\n", id, shared_data);
        usleep(2000000);
        pthread_rwlock_unlock(&rw_lock);
        usleep(1500000);
    }
    free(arg);
    return NULL;
}

int main(void)
{
    pthread_t threads[NUM_READERS + NUM_WRITERS];

    signal(SIGINT, signal_handler);
    printf("User-space threads initialization...\n");
    pthread_rwlock_init(&rw_lock, NULL);

    for(int i = 0; i<NUM_READERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&threads[i], NULL, reader_thread, id);
    }

    for(int i = 0; i < NUM_WRITERS; i++){
        int *id = malloc(sizeof(int));
        *id = i + 1;

        pthread_create(&threads[NUM_READERS + i], NULL, writer_thread, id);
    }

    printf("%d threads created\n", NUM_READERS + NUM_WRITERS);
	printf("Press Ctrl+C to stop...\n");

    for(int i = 0;i<NUM_READERS + NUM_WRITERS; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_rwlock_destroy(&rw_lock);
    printf("All threads finished\n");
    return 0;
}