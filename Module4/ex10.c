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



#define BUFFER_SIZE 10
#define NUM_ITEMS 30

typedef struct {
	int buffer[BUFFER_SIZE];
	int head;
	int tail;
	int full_count; 
	int empty_count; 
} shared_data;


int main(int argc, char *argv[]){
	int fd;
	shared_data *sData;
	pid_t producer_pid, consumer_pid;
	
	if((fd = shm_open("/ex10", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) == -1){
		perror("shm_open error");
		exit(1);
	}
	
	if(ftruncate(fd, sizeof(shared_data))== -1){
		perror("ftruncate error");
		exit(2);
	}
	
	if((sData = (shared_data*)mmap(0, sizeof(shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0))== MAP_FAILED){
		perror("nmaps error");
		exit(3);
	}
	
	sData->head = 0;
	sData->tail = 0;
	sData->full_count = 0;
	sData->empty_count = BUFFER_SIZE;
	
	producer_pid = fork();
	if (producer_pid == 0) {
		for (int i = 0; i < NUM_ITEMS; i++) {
			while (sData->empty_count == 0);

			
			sData->buffer[sData->tail] = i + 1;
			printf("Produced: %d\n", i + 1);

			
			sData->tail = (sData->tail + 1) % BUFFER_SIZE;

			sData->full_count++;
			sData->empty_count--;

		}
		exit(0);
	}
	consumer_pid = fork();
	if (consumer_pid == 0) { 
		for (int i = 0; i < NUM_ITEMS; i++) {
			while (sData->full_count == 0);

			int item = sData->buffer[sData->head];
			printf("Consumed: %d\n", item);

			sData->head = (sData->head + 1) % BUFFER_SIZE;

			sData->full_count--;
			sData->empty_count++;
		}
		exit(0);
	}

	wait(NULL);
	wait(NULL);
	

	if (munmap(sData, sizeof(shared_data)) == -1) {
		perror("munmap failed");
		exit(1);
	}
	if (close(fd) == -1) {
		perror("close failed");
		exit(1);
	}
	
	if (shm_unlink("/ex10") == -1) {
		perror("shm_unlink failed");
	}

	return 0;
}