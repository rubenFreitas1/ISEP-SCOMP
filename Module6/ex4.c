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
#define THREAD_SEARCH 20
#define THREADS 5
int expectedNumber;
int array[SIZE];
char str[SIZE];


void* thread_function(void* arg){
    int threadNumber = *((int*)arg);
    free(arg);
    int start = threadNumber*THREAD_SEARCH;
    int end = start + THREAD_SEARCH;
    for(int i = start; i < end; i++){
        if(expectedNumber == array[i]){
            snprintf(str, SIZE, "[THREAD] FOUND NUMBER ON POSITION %d \n", i);
            write(STDOUT_FILENO, str, strlen(str));
            int *result = malloc(sizeof(int));
            *result = threadNumber+1;
            pthread_exit(result);
        }
    }
    pthread_exit((void*)NULL);
}

int main(){
    pthread_t threads[THREADS];
    srand(time(NULL));
    int used[201] = {0};

    for(int i = 0; i < SIZE; i++){
        int num;
        do{
            num = (rand() % 200) +1;
        }while(used[num] != 0);
        array[i] = num;
        used[num] = 1;
    }

    expectedNumber = array[rand() % SIZE];
    printf("[PARENT] Number to find: %d\n", expectedNumber);

    for(int i = 0; i < THREADS; i++){
        int *arg = malloc(sizeof(int));
        if(arg == NULL){
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        *arg = i;
        if(pthread_create(&threads[i], NULL , thread_function, (void*)arg) == -1){
            perror("pthread create failed");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < THREADS; i++){
        int *exitCode;
        pthread_join(threads[i],(void*)&exitCode);
        if(exitCode != NULL){
            printf("[PARENT] THREAD %d FOUND THE NUMBER\n", *exitCode);
            free(exitCode);
        }
    }

    return 0;
}