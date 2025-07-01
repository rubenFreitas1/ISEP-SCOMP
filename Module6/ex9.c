#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#define SIZE 100
#define SIZE_PER_THREAD 20
#define SIZE_STR 1000
#define THREADS 5

pthread_mutex_t mutex;
pthread_cond_t can_print;
int ind = 0;
char str[SIZE_STR];
int result[SIZE];
int data[SIZE];

void* thread_function(void* arg){
    int number = *((int*)arg);
    free(arg);
    
    int start = number * SIZE_PER_THREAD;
    int end = start + SIZE_PER_THREAD;

    for(int i = start; i < end; i++){
        result[i] = data[i] * 10 + 2;
    }

    pthread_mutex_lock(&mutex);

    while(number != ind){
        pthread_cond_wait(&can_print, &mutex);
    }

    for(int i = start; i < end; i++){
        snprintf(str, SIZE_STR, "[THREAD %d] %d\n", number+1 ,result[i]);
        write(STDOUT_FILENO, str, strlen(str));
    }
    ind++;

    pthread_cond_broadcast(&can_print);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}



int main(){
    pthread_t threads[THREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&can_print, NULL);
    memset(result, 0, sizeof(result));
    for(int i = 0; i < SIZE; i++){
        data[i] = 1;
    }


    for(int i = 0; i < THREADS; i++){
        int *number = malloc(sizeof(int));
        *number = i;
        if( (pthread_create(&threads[i], NULL, thread_function, (void*)number)) != 0 ){
            perror("pthread create failed");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&can_print);

    return 0;
}