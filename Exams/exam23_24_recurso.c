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

int gera_num(int inicio, int fim){
	int num = (rand() % (fim-inicio+1)) + inicio;
    return num;
}


typedef struct{
	int number;
	int event_scores[3];
	int total_time;
	float final_score;
}athlete_score;


pthread_mutex_t mutex;
pthread_cond_t can_calculate, can_generate;
int ind = 0;
int done_generating = 0;
int done_calculating = 1;
athlete_score scores[300];
char str[500];


void* t1_thread (void *arg){
	for(int i = 0; i < 300; i++){
		pthread_mutex_lock(&mutex);

		while(done_calculating == 0){
			pthread_cond_wait(&can_generate,&mutex);
		}
		done_calculating = 0;
		
		ind = i;
		for(int j = 0; j < 3; j++){
			scores[i].event_scores[j] = gera_num(0, 100); 
		}
		scores[i].total_time = gera_num(50, 100);
		
		snprintf(str, 500, "[T1] GENERATED DATA FOR ATHLETE %d \n", ind+1);
		write(STDOUT_FILENO, str, strlen(str));

		done_generating = 1;
		pthread_cond_signal(&can_calculate);
		
		pthread_mutex_unlock(&mutex);
		
	}

	pthread_exit(NULL);
}

void* t2_thread (void *arg){
	for(int i = 0; i < 300; i++){
		pthread_mutex_lock(&mutex);
		
		while(done_generating == 0){
			pthread_cond_wait(&can_calculate, &mutex);
		}
		done_generating = 0;
		
		float sc_swim =(float) scores[ind].event_scores[0];
		float sc_bike =(float) scores[ind].event_scores[1];
		float sc_run =(float) scores[ind].event_scores[2];
		float total_time = (float) scores[ind].total_time;
		
		float final_score = 0.30 *(sc_swim + sc_bike + sc_run)/3 + 0.7*total_time;
		scores[ind].final_score = final_score;
		snprintf(str, 500, "[T2] CALCULATED FINAL SCORE (%.2f) FOR ATHLETE %d \n", final_score ,ind+1);
		write(STDOUT_FILENO, str, strlen(str));

		done_calculating = 1;
		pthread_cond_signal(&can_generate);
		pthread_mutex_unlock(&mutex);	
	}

	pthread_exit(NULL);
}


int main(){
	pthread_t t1;
	pthread_t t2;
    srand(time(NULL));

	for(int i = 0; i < 300; i++){
		scores[i].number = i+1;
	}

	pthread_cond_init(&can_calculate, NULL);
	pthread_cond_init(&can_generate, NULL);
	pthread_mutex_init(&mutex, NULL);

	if(pthread_create(&t1, NULL, t1_thread, NULL) != 0){
		perror("thread create t1 failed");
		exit(EXIT_FAILURE);
	}

	if(pthread_create(&t2, NULL, t2_thread, NULL) != 0){
		perror("thread create t2 failed");
		exit(EXIT_FAILURE);
	}

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

	pthread_cond_destroy(&can_calculate);
	pthread_cond_destroy(&can_generate);
	pthread_mutex_destroy(&mutex);

	return 0;
}

