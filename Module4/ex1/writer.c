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
    shared_student_info *sInfo;
    int fd;

    if((fd = shm_open("/ex1", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) == -1){
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    if((ftruncate(fd, sizeof(shared_student_info))) == -1){
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    if((sInfo = (shared_student_info*)mmap(0,sizeof(shared_student_info),PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    sInfo->written= 0;

    printf("Number: ");
	scanf("%d", &sInfo->number);
	
	
	printf("Name: ");
	scanf("%s", sInfo->name);
	
	printf("Address: ");
	scanf("%s", sInfo->address);

    sInfo->written = 1;

    printf("Number: %d \t Name: %s \t Address: %s \n", sInfo->number, sInfo->name, sInfo->address);
    return 0;

}