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

#define SIZE 500
#define MAX_SIZE 4096

typedef struct{
	pthread_t threadID;
	int hour;
	char string1[SIZE];
    char string2[SIZE];
}Shared_data;


#define WRITERS 5
#define READERS 5

pthread_mutex_t mutex;
pthread_cond_t can_read, can_write;
int readers_count = 0;
int writer_active = 0;
int fd;
char str[MAX_SIZE];
Shared_data *data;




void* thread_writers(void *arg){

	pthread_mutex_lock(&mutex);

	while(readers_count > 0 || writer_active > 0){
		pthread_cond_wait(&can_write, &mutex);
	}

	writer_active = 1;
	srand(time(NULL));
	snprintf(data->string1, SIZE, "THREAD ID: %ld", pthread_self());
	snprintf(data->string2, SIZE, "HOUR: %d", rand()%24);

	snprintf(str, SIZE, "WRITES: %d || ACIVE READERS: %d \n", writer_active, readers_count);
	write(STDOUT_FILENO, str, strlen(str));
	
	writer_active = 0;
	
	pthread_cond_broadcast(&can_read);

	pthread_mutex_unlock(&mutex);
	
	pthread_exit(NULL);
}

void* thread_readers(void *arg){

	pthread_mutex_lock(&mutex);
	

	while(writer_active){
		pthread_cond_wait(&can_read, &mutex);
	}

	readers_count++;
	

	pthread_mutex_unlock(&mutex);

    snprintf(str, MAX_SIZE, "Active Readers : %d\n", readers_count);
	write(STDOUT_FILENO, str, strlen(str));	

	snprintf(str, MAX_SIZE, "String1 : %s\n String2: %s\n", data->string1, data->string2);
	write(STDOUT_FILENO, str, strlen(str));

	pthread_mutex_lock(&mutex);
	
	readers_count--;
	
	if(readers_count == 0){
		pthread_cond_signal(&can_write);
	}

	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}


int main(){
  pthread_t writers[WRITERS];
  pthread_t readers[READERS];

  if( (fd = shm_open("/modelo", O_CREAT|O_RDWR, S_IWUSR|S_IRUSR)) == -1 ){
	perror("shm open failed");
	exit(EXIT_FAILURE);
  }

  if((ftruncate(fd, sizeof(Shared_data))) == -1){
	perror("ftruncate failed");
	exit(EXIT_FAILURE);
  }

  if( (data = (Shared_data*)mmap(data, sizeof(Shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED ){
	perror("mmap failed");
	exit(EXIT_FAILURE);
  }

  if(pthread_mutex_init(&mutex, NULL) != 0){
	perror("mutex init error");
	exit(EXIT_FAILURE);
  }

  pthread_cond_init(&can_write, NULL);
  pthread_cond_init(&can_read, NULL);

  for(int i = 0; i < WRITERS; i++){
	if (pthread_create(&writers[i], NULL, thread_writers, NULL) != 0){
             perror("pthread create writers failed");
	     exit(EXIT_FAILURE);	
	}
  }

  for(int i = 0; i < READERS; i++){
	if (pthread_create(&readers[i], NULL, thread_readers, NULL) != 0){
             perror("pthread create readers failed");
	     exit(EXIT_FAILURE);	
	}

  }
  

  for(int i = 0; i < WRITERS; i++){
	pthread_join(writers[i], NULL); 
  }

  for(int i = 0; i < READERS; i++){
	pthread_join(readers[i], NULL); 
  }
  
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&can_read);
  pthread_cond_destroy(&can_write);
  

  if(munmap(data,sizeof(Shared_data)) == -1){
	perror("munmap failed");
	exit(EXIT_FAILURE);
  }

  if(close(fd) == -1){
	perror("close failed");
	exit(EXIT_FAILURE);
  }

  if(shm_unlink("/modelo") == -1){
	perror("unlink failed");
	exit(EXIT_FAILURE);
  }

  return 0;
}