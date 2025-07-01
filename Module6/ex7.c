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

sem_t *sem_barrier, *sem_ready;
pthread_t t1, t2;
char str[SIZE_STR];

void buy_beer(){
	snprintf(str, SIZE_STR, "BUYING BEER \n");
	write(STDOUT_FILENO, str, strlen(str));
}


void buy_chips(){
	snprintf(str, SIZE_STR, "BUYING CHIPS \n");
	write(STDOUT_FILENO, str, strlen(str));
}

void eat_and_drink(){
	snprintf(str, SIZE_STR, "EATING AND DRINKING \n");
	write(STDOUT_FILENO, str, strlen(str));
}


void* t1_thread(void *arg ){
	
	buy_chips();
	sem_post(sem_ready);
	sem_wait(sem_barrier);
	eat_and_drink();
	pthread_exit(NULL);
}


void* t2_thread(void *arg ){
	
	buy_beer();
	sem_post(sem_ready);
	sem_wait(sem_barrier);
	eat_and_drink();
	pthread_exit(NULL);
}


int main(){

    if( (sem_ready = sem_open("/sem_ready", O_CREAT, 0644, 0)) == SEM_FAILED ){
		perror("sem open ready failed");
		exit(EXIT_FAILURE);
	}

	if( (sem_barrier = sem_open("/sem_barrier", O_CREAT, 0644, 0)) == SEM_FAILED ){
		perror("sem open barrier failed");
		exit(EXIT_FAILURE);
	}
	
	if(pthread_create(&t1, NULL, t1_thread, NULL ) != 0){
		perror("pthread create t1 failed");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&t2,NULL, t2_thread, NULL ) != 0){
		perror("pthread create t2 failed");
		exit(EXIT_FAILURE);
	}

    sem_wait(sem_ready);
    sem_wait(sem_ready);

    sem_post(sem_barrier);
    sem_post(sem_barrier);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	sem_close(sem_barrier);
	sem_close(sem_ready);
	sem_unlink("/sem_barrier");
	sem_unlink("/sem_ready");

	return 0;
}

