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



#define SIZE_STR 500
#define SIZE_BUFFER 4
#define CLIENTS 5
#define PRINTERS 3
int numberJobs = 0;
int head = 0;  // índice para remover
int tail = 0;  // índice para adicionar
pthread_mutex_t mutex;
pthread_cond_t can_add , can_remove;

typedef struct{
	int clientID;
	char content[SIZE_STR];
}Print;

Print* jobQueue;




void* client_thread(void* arg){
	int id = *((int*)arg);
	free(arg);
	while(1){
	
		pthread_mutex_lock(&mutex);

		while(numberJobs == SIZE_BUFFER){
			pthread_cond_wait(&can_add,&mutex);
		}
		

		Print newJob;
	
		newJob.clientID = id;
		snprintf(newJob.content, SIZE_STR, "Job do cliente %d", id);

		jobQueue[tail] = newJob;
		tail = (tail + 1) % SIZE_BUFFER;
		numberJobs++;

		printf("Cliente %d adicionou um trabalho. Jobs na fila: %d\n", id, numberJobs);

		pthread_cond_signal(&can_remove);
		pthread_mutex_unlock(&mutex);
		sleep(2);
	}

	pthread_exit(NULL);
}

void* printer_thread(void* arg){
	int id = *((int*)arg);
	free(arg);
	while(1){

		pthread_mutex_lock(&mutex);
		
		while(numberJobs == 0){
			pthread_cond_wait(&can_remove, &mutex);
		}
		
		Print job = jobQueue[head];
		head = (head + 1) % SIZE_BUFFER;
		numberJobs--;
	
		printf("Impressora %d vai imprimir job do cliente %d. Jobs restantes: %d\n", id, job.clientID, numberJobs);

		pthread_cond_signal(&can_add);

		pthread_mutex_unlock(&mutex);
		sleep(5);
		printf("Impressora %d terminou job do cliente %d\n", id, job.clientID);

	}

	pthread_exit(NULL);
}





int main(){

	pthread_t printers[PRINTERS];
	pthread_t clients[CLIENTS];
	
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&can_add, NULL);
	pthread_cond_init(&can_remove, NULL);

	jobQueue = malloc(SIZE_BUFFER * sizeof(Print));
	
	for(int i = 0; i < CLIENTS; i++){
		int* number = malloc(sizeof(int));
		*number = i;
		if( pthread_create(&clients[i], NULL, client_thread,(void*)number) != 0){
			perror("pthread create clients failed");
			exit(EXIT_FAILURE);
		}
	}

	for(int i = 0; i < PRINTERS; i++){
		int *id = malloc(sizeof(int));
		*id = i;
		if( pthread_create(&printers[i], NULL, printer_thread, (void*)id) != 0){
			perror("pthread create printers failed");
			exit(EXIT_FAILURE);
		}
	}


	for(int i = 0; i < CLIENTS; i++){
		pthread_join(clients[i], NULL);
	}

	for(int i = 0; i < PRINTERS; i++){
		pthread_join(printers[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&can_add);
	pthread_cond_destroy(&can_remove);

	return 0;
}
