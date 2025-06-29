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

#define SIZE 200
#define THREADS 5
pthread_mutex_t mutex;

void* thread_function(void *arg){
    int number = *((int*)arg);
    int start= number* SIZE;
    int end = start + SIZE;
    pthread_mutex_lock(&mutex);
    
    FILE* file = fopen("teste.txt", "a");
    if(file == NULL){
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }
    for(int i = start; i < end; i++){
        fprintf(file, "Thread %d: %d\n", number+1, i);
    }
    fclose(file);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(){
    pthread_t threads[THREADS];

    if(pthread_mutex_init(&mutex, NULL) != 0){
        perror("mutex init error");
        exit(EXIT_FAILURE);
    }

    FILE* file = fopen("teste.txt", "w");
    if(file == NULL){
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fclose(file);

    for(int i = 0; i < THREADS; i++){
        int* number = malloc(sizeof(int));
        *number = i;
        pthread_create(&threads[i], NULL, thread_function, (void*)number);
    }

    for(int i = 0; i < THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    file = fopen("teste.txt", "r");
    if(file == NULL){
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    printf("\nConteúdo final do ficheiro:\n");
    char line[256];
    while(fgets(line, sizeof(line), file)){
        printf("%s", line);
    }
    fclose(file);

}