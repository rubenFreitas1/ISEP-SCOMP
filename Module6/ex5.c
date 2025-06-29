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

#define SIZE 1000
#define THREADS 3
int max;
int min;
float average;
int array[SIZE];

void* lowest_balance(void *arg){
    min = array[0];
    for(int i = 0; i < SIZE; i++){
        if(array[i] < min){
            min = array[i];
        }
    }
    pthread_exit(NULL);
}

void* highest_balance(void *arg){
    max = array[0];
    for(int i = 0; i < SIZE; i++){
        if(array[i] > max){
            max = array[i];
        }
    }
    pthread_exit(NULL);
}

void* average_balance(void *arg){
    average = 0;
    for(int i = 0; i < SIZE; i++){
        average +=(float) array[i];
    }
    average = average / (float) SIZE;

    pthread_exit(NULL);
}


int main(){
    pthread_t threads[THREADS];
    srand(time(NULL));
    for(int i = 0; i < SIZE; i++){
        array[i] = (rand() % 1500) + 1;
    }

    pthread_create(&threads[0], NULL, lowest_balance, NULL);
    pthread_create(&threads[1], NULL, highest_balance, NULL);
    pthread_create(&threads[2], NULL, average_balance, NULL);
    for(int i = 0; i < THREADS; i++){
        pthread_join(threads[i], NULL);
    }
    printf("MIN: %d || MAX: %d || AVERAGE: %.2f\n", min, max, average);
    return 0;

}
