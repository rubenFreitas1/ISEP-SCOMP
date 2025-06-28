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

#define MAX_STR 100

typedef struct{
	int written;
	int number;
	char name[MAX_STR];
	char address[MAX_STR];
}shared_student_info;



int main(){
    int fd;
    shared_student_info *sInfo;
    char local_name[MAX_STR], local_address[MAX_STR];
	int local_number;

    if( (fd = shm_open("/ex1", O_RDWR, 0)) == -1 ){
       perror("shm_open error");
		exit(EXIT_FAILURE); 
    }

    if( (sInfo = (shared_student_info*)mmap(0, sizeof(shared_student_info), PROT_READ|PROT_WRITE, MAP_SHARED, fd,0) ) == MAP_FAILED){
        perror("mmap error");
		exit(EXIT_FAILURE); 
    }

    while(sInfo->written == 0){}
    

    strcpy(local_name, sInfo->name);
	strcpy(local_address, sInfo->address);
	local_number = sInfo->number;
	printf("Number: %d \t Name: %s \t Address: %s \n", local_number, local_name, local_address);

	return 0;

}