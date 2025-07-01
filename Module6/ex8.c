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

#define SIZE_STR 1000
#define THREADS 6
sem_t *sem_barrier, *sem_ready;
pthread_t t1, t2;
char str[SIZE_STR];


void buy_beer(int number){
	snprintf(str, SIZE_STR, "[THREAD %d] BUYING BEER \n", number);
	write(STDOUT_FILENO, str, strlen(str));
}


void buy_chips(int number){
	snprintf(str, SIZE_STR, "[THREAD %d] BUYING CHIPS \n", number);
	write(STDOUT_FILENO, str, strlen(str));
}

void eat_and_drink(int number){
	snprintf(str, SIZE_STR, "[THREAD %d] EATING AND DRINKING \n", number);
	write(STDOUT_FILENO, str, strlen(str));
}


void* thread_function(void *arg){
    int number = *((int*)arg);
    free(arg);
	
    int choice = (rand() % 2) + 1;
    if(choice == 1){
        buy_chips(number);
    }else{
        buy_beer(number);
    }
	sem_post(sem_ready);
	sem_wait(sem_barrier);
	eat_and_drink(number);
	pthread_exit(NULL);
}



int main(){
    pthread_t threads[THREADS];
    srand(time(NULL));
    if( (sem_ready = sem_open("/sem_ready", O_CREAT, 0644, 0)) == SEM_FAILED ){
		perror("sem open ready failed");
		exit(EXIT_FAILURE);
	}

	if( (sem_barrier = sem_open("/sem_barrier", O_CREAT, 0644, 0)) == SEM_FAILED ){
		perror("sem open barrier failed");
		exit(EXIT_FAILURE);
	}

    for(int i = 0; i < THREADS; i++){
        int *number = malloc(sizeof(int));
        *number = i+1;
        if(pthread_create(&threads[i], NULL, thread_function, (void*)number ) != 0){
		    perror("pthread create failed");
		    exit(EXIT_FAILURE);
	    }
    }

    for(int i = 0; i < THREADS; i++){
        sem_wait(sem_ready);
    }

    for(int i = 0; i < THREADS; i++){
        sem_post(sem_barrier);
    }

    for(int i = 0; i < THREADS; i++){
       pthread_join(threads[i], NULL);
	}

	sem_close(sem_barrier);
	sem_close(sem_ready);
	sem_unlink("/sem_barrier");
	sem_unlink("/sem_ready");

	return 0;
}

