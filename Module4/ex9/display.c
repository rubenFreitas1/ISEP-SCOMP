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
	
	if((fd = shm_open("/ex9", O_RDWR, S_IRUSR|S_IWUSR)) == -1){
		perror("shm_open error");
		exit(1);
	}
	
	if((sData = (shared_data*)mmap(0, sizeof(shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0))== MAP_FAILED){
		perror("nmaps error");
		exit(3);
	}
	
	while(sData->flag_encrypt == 0);
	
	for(int i = 0; i < MESSAGES; i++){
		if(sData->flag_encrypt == 1){
			
			for (int i = 0; sData->message[i] != '\0'; i++) {
				sData->message[i] -= sData->random_value;
			}
			printf("Display message: %s\n",sData->message);
			
			sData->flag_write = 0;
			sData->flag_encrypt = 0;
			sData->flag_display = 1;
			
			while(sData->flag_encrypt == 0);
		}
	}
	

	if (munmap(sData, sizeof(shared_data)) == -1) {
		perror("munmap failed");
		exit(1);
	}
	if (close(fd) == -1) {
		perror("close failed");
		exit(1);
	}
	
	if (shm_unlink("/ex9") == -1) {
		perror("shm_unlink failed");
	}
	return 0;
}
