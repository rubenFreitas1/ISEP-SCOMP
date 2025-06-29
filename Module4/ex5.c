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

#define SIZE 1000

typedef struct{
    int max[10];
}shared_data;


int main(){
    int fd;
    int v[SIZE];
    srand(time(NULL));
    for(int i = 0; i < SIZE; i++){
        v[i] = (rand() % 1000) +1;
    }
    pid_t pids[10];
    shared_data *sharedData;

    if((fd = shm_open("/ex5", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1 ){
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if((ftruncate(fd, sizeof(shared_data))) == -1){
        perror("ftruncate failed");
        exit(EXIT_FAILURE); 
    }

    if( (sharedData=(shared_data*)mmap(sharedData, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 10; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(pids[i] == 0){
            int start = i * 100;
            int end = start + 100;
            int max = 0;
            for(int j= start; j < end; j++){
                if(v[j] > max){
                    max = v[j];
                }
            }
            sharedData->max[i]= max;
            exit(1);
        }
    }
    for(int i= 0; i < 10; i++){
        wait(NULL);
    }
    int greatest = 0;
    for(int i = 0; i < 10; i++ ){
        if(sharedData->max[i] > greatest){
            greatest = sharedData->max[i];
        }
    }
    printf("Greatest number: %d\n", greatest);

    if((munmap(sharedData, sizeof(shared_data))) == -1){
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    if(close(fd) == -1){
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    if((shm_unlink("/ex5")) == -1){
        perror("unlink failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}