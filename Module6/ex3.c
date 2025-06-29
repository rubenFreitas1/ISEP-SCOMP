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

#define NAME 100
#define THREADS 5 
#define STR_SIZE 1000
char str[STR_SIZE];

typedef struct{
    int number;
    char name[NAME];
    int grade;
}struct_data;


void* thread_function(void* arg){
    struct_data* data = (struct_data*)arg;
    
    snprintf(str, STR_SIZE, "Name: %s || Number: %d || Grade: %d \n", data->name, data->number, data->grade);
    write(STDOUT_FILENO, str, strlen(str));

    pthread_exit(NULL);
}


int main(){
    pthread_t threads[THREADS];
    struct_data array[THREADS];
    strcpy(array[0].name, "Ruben");
    array[0].number = 5;
    array[0].grade = 20;

    strcpy(array[1].name, "João");
    array[1].number = 10;
    array[1].grade = 18;

    strcpy(array[2].name, "Ana");
    array[2].number = 15;
    array[2].grade = 19;

    strcpy(array[3].name, "Maria");
    array[3].number = 20;
    array[3].grade = 17;

    strcpy(array[4].name, "Luis");
    array[4].number = 25;
    array[4].grade = 16;

    for(int i = 0; i < THREADS; i++){
        if( pthread_create(&threads[i], NULL, thread_function, (void*)&array[i]) == -1){
            perror("pthread create failed");
            exit(EXIT_FAILURE);
        }
    }

    for(int i = 0; i < THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}