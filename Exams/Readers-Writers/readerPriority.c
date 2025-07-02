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


pthread_mutex_t wrt, mutex;
int readers_count = 0;
int writer = 0;
#define SIZE_STR 500
#define MAX_SIZE 2000
#define READERS 10
#define WRITERS 5

typedef struct{
    char string1[SIZE_STR];
    char string2[SIZE_STR];
}shared_data;

shared_data *data;


void* thread_writers(void *arg){

    pthread_mutex_lock(&wrt);
    writer = 1;
    srand(time(NULL));
	snprintf(data->string1, SIZE_STR, "THREAD ID: %ld", pthread_self());
	snprintf(data->string2, SIZE_STR, "HOUR: %d", rand()%24);

    char str[SIZE_STR];
	snprintf(str, SIZE_STR, "WRITES: %d || ACIVE READERS: %d \n", writer, readers_count);
	write(STDOUT_FILENO, str, strlen(str));
    writer = 0;
    pthread_mutex_unlock(&wrt);

    pthread_exit(NULL);
}



void* thread_readers(void *arg){

    pthread_mutex_lock(&mutex);
        readers_count ++;
        if(readers_count == 1){
            pthread_mutex_lock(&wrt);
        }
        
    pthread_mutex_unlock(&mutex);

    char str[MAX_SIZE];
    snprintf(str, MAX_SIZE, "Active Readers : %d\n", readers_count);
	write(STDOUT_FILENO, str, strlen(str));	

	snprintf(str, MAX_SIZE, "String1 : %s\n String2: %s\n", data->string1, data->string2);
	write(STDOUT_FILENO, str, strlen(str));

    pthread_mutex_lock(&mutex);
        readers_count--;
        if(readers_count == 0){
            pthread_mutex_unlock(&wrt);
        }

    pthread_mutex_unlock(&mutex);


    pthread_exit(NULL);
}



int main(){
    pthread_t writers[WRITERS];
    pthread_t readers[READERS];
    data = malloc(sizeof(shared_data));
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&wrt, NULL);

    for(int i = 0; i < WRITERS; i++){
        if( pthread_create(&writers[i], NULL, thread_writers, NULL)  != 0){
            perror("pthread create writers failed");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < READERS; i++){
        if( pthread_create(&readers[i], NULL, thread_readers, NULL)  != 0){
            perror("pthread create readers failed");
            exit(EXIT_FAILURE);
        }
    }


    for(int i = 0; i < WRITERS; i++){
        pthread_join(writers[i],NULL);
    }

    for(int i = 0; i < READERS; i++){
        pthread_join(readers[i],NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&wrt);
    free(data);
    return 0;
}
