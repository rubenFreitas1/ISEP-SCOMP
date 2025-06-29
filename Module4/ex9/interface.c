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

#define STR_SIZE 50
#define MESSAGES 3

typedef struct{
char message[STR_SIZE];
int flag_write;
int flag_encrypt;
int flag_display;
int random_value;
}shared_data;


int main(int argc, char *argv[]){
	int fd;
	shared_data *sData;
	char input[STR_SIZE];
	
	if((fd = shm_open("/ex9", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) == -1){
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
	
	sData->flag_write= 0;
	sData->flag_display = 0;
	sData->flag_encrypt = 0;
	
	for(int i = 0; i < MESSAGES; i++){
		printf("Enter a message: ");
		fgets(input, STR_SIZE, stdin);
		input[strcspn(input, "\n")] = '\0';

		strncpy(sData->message, input, STR_SIZE);
		
		sData->flag_encrypt = 0;
		sData->flag_display = 0;
		sData->flag_write = 1;
		
		while(sData->flag_display == 0);
		sData->message[0] = '\0';
	}
	
	sData->flag_display = 1;
	sData->flag_write = 1;
	sData->flag_encrypt = 1;

	if (munmap(sData, sizeof(shared_data)) == -1) {
		perror("munmap failed");
		exit(1);
	}
	if (close(fd) == -1) {
		perror("close failed");
		exit(1);
	}

	return 0;
}
