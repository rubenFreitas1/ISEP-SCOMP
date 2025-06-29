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


#define SPACE 10
#define NUM_SEQUENCE 5

typedef struct{
    int flag;
    int array[SPACE];
}shared_data;

int main(){
    shared_data *sharedData;
    int fd;

    if( (fd = shm_open("/ex3", O_RDWR, 0)) == -1 ){
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if( (sharedData=(shared_data*)mmap(sharedData, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    for(int w = 0; w < NUM_SEQUENCE; w++){
        while(sharedData->flag != 1);
        float average = 0;
        for(int i = 0; i < SPACE; i++){
            average+= sharedData->array[i];
        }
        average /= (float) SPACE;
        printf("Average: %f \n", average);
        sharedData->flag = 0;
    }

    if( munmap(sharedData, sizeof(shared_data)) == -1){
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    if(close(fd) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }

    if( shm_unlink("/ex3") == -1){
        perror("unlink failed");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}