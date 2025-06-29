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

#define MAX_MESSAGE_LENGTH 100
#define CHILDS 5


typedef struct{
	int turn;
	char string[MAX_MESSAGE_LENGTH];
}shared_data;

void append_message(char *message, const char *word) {
	if (strlen(message) + strlen(word) + 1 < MAX_MESSAGE_LENGTH) {
		strcat(message, word);
		strcat(message, " ");
	}
}


int main(int argc, char *argv[]){
	int fd;
	shared_data *sData;
	pid_t pids[CHILDS];

	if((fd = shm_open("/ex6", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) == -1){
		perror("shm_open error");
		exit(EXIT_FAILURE);
	}
	
	if(ftruncate(fd, sizeof(shared_data))== -1){
		perror("ftruncate error");
		exit(EXIT_FAILURE);
	}
	
	if((sData = (shared_data*)mmap(0, sizeof(shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0))== MAP_FAILED){
		perror("nmaps error");
		exit(EXIT_FAILURE);
	}
	
	sData->string[0] = '\0';
	sData->turn = 0;

	for(int i = 0; i < CHILDS; i++){
		pids[i] = fork();
		
		if (pids[i] == -1) {
			perror("fork failed");
			exit(1);
		}
		
		if(pids[i] == 0){
			char word[50];
			while(1){
				if(sData->turn == i){
					printf("Child %d, enter a word: ", i + 1);
					scanf("%s", word);
					append_message(sData->string, word);
					sData->turn++;
					break;
				}
			}
			exit(EXIT_SUCCESS);	
		}
	}
	
	for(int i = 0; i < CHILDS; i++){
		waitpid(pids[i], NULL, 0);
	}
	
	printf("\nFinal message: %s\n", sData->string);

	if (munmap(sData, sizeof(shared_data)) == -1) {
		perror("munmap failed");
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1) {
		perror("close failed");
		exit(EXIT_FAILURE);
	}
    if((shm_unlink("/ex6")) == -1){
        perror("close failed");
		exit(EXIT_FAILURE);
    }
	
	return 0;
}