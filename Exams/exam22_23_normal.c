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


#define NVOLTAS 5
#define NCARROS 5

int readyToRace = 0;
int raceStarted = 0;
int carsFinished = 0;
pthread_cond_t ready, startRace, raceEnd;
pthread_mutex_t mutex;
char str[500];


typedef struct{
	int number;
	int total_time;
}car_data;

car_data carros[NCARROS];



void* thread_carro(void* arg){
	int numberCar = *((int*)arg);
	free(arg);

	pthread_mutex_lock(&mutex);
	readyToRace++;
	pthread_cond_signal(&ready);
	while(raceStarted == 0){
		pthread_cond_wait(&startRace, &mutex);
	}

	pthread_mutex_unlock(&mutex);
    char local_str[100];

	for(int i = 0; i < NVOLTAS; i++){
		int tempoVolta = (rand() % 10) + 5;
		carros[numberCar].total_time += tempoVolta;
		
		sleep(tempoVolta);
		snprintf(local_str, 100, "[CAR %d] FINISHED LAP %d \n", carros[numberCar].number, i+1);
		write(STDOUT_FILENO, local_str, strlen(local_str));

	}

	pthread_mutex_lock(&mutex);

	carsFinished++;
	snprintf(local_str, 100, "[CAR %d] FINISHED THE RACE \n", carros[numberCar].number);
	write(STDOUT_FILENO, local_str, strlen(local_str));
	
	pthread_cond_signal(&raceEnd);

	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}


void* controlo_corrida(void *arg){

	pthread_mutex_lock(&mutex);
		
		while(readyToRace != NCARROS){
			pthread_cond_wait(&ready, &mutex);
		}
		raceStarted = 1;
		pthread_cond_broadcast(&startRace);

		snprintf(str, 500, "[CAR CONTROL] RACE START!! \n");
		write(STDOUT_FILENO, str, strlen(str));
		
	
		while(carsFinished != NCARROS){
			pthread_cond_wait(&raceEnd, &mutex);
		}

		snprintf(str, 500, "[CAR CONTROL] RACE FINISHED!! \n");
		write(STDOUT_FILENO, str, strlen(str));

	pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}



int main(){
	for(int i = 0; i < NCARROS; i++){
		carros[i].number = i + 1;
	}

	pthread_t carros[NCARROS];
	pthread_t controlo_carros;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&ready, NULL);
	pthread_cond_init(&startRace, NULL);
	pthread_cond_init(&raceEnd, NULL);

	if( pthread_create(&controlo_carros, NULL, controlo_corrida, NULL) != 0){
		perror("pthread create controlo_carros failed");
		exit(EXIT_FAILURE);
	}


	for(int i = 0; i < NCARROS; i++){
		int *number = malloc(sizeof(int));
		*number = i;
		if( pthread_create(&carros[i], NULL, thread_carro, (void*)number) != 0){
			perror("pthread create carros failed");
			exit(EXIT_FAILURE);
		}
	}


	for(int i = 0; i < NCARROS; i++){
		pthread_join(carros[i], NULL);
	}
	pthread_join(controlo_carros, NULL);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&ready);
	pthread_cond_destroy(&startRace);
	pthread_cond_destroy(&raceEnd);
	return 0;
}
