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

typedef struct{
    int flag;
    int array[SPACE];
}shared_data;

int main(){
    shared_data *sharedData;
    int fd;
    
    if( (fd = shm_open("/ex3", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if((ftruncate(fd, sizeof(shared_data))) == -1 ){
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    if( (sharedData=(shared_data*)mmap(0, sizeof(shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED ){
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    sharedData->flag = 0;
    srand(time(NULL));
    for(int i = 0; i < SPACE; i++){
        sharedData->array[i] = (rand() % 20) + 1;
        printf("Número gerado [%d]: %d\n", i, sharedData->array[i]);
    }

    sharedData->flag = 1;

    if( munmap(sharedData, sizeof(shared_data)) == -1){
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    if(close(fd) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;

}