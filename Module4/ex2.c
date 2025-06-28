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

//a) Se não usarmos uma flag para sincronizar os processos o resultado vai ser imprevisivel.

typedef struct{
    int flag;
    int num1;
    int num2;
}shared_data;

int main(){
    shared_data *sharedData;
    int fd;
    pid_t pid;  

    if( (fd = shm_open("/ex2", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR)) == -1 ){
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if( (ftruncate(fd, sizeof(shared_data))) == -1 ){
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    if( (sharedData = (shared_data*)mmap(0, sizeof(shared_data), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0) ) == MAP_FAILED){
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    sharedData->num1 = 10000;
    sharedData->num2 = 500;
    sharedData->flag = 0;

    pid = fork();
    if(pid == -1){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if(pid == 0){
        for(int i = 0; i < 100000; i++){
            while (sharedData->flag != 1);
            sharedData->num1++;
            sharedData->num2--;
            sharedData->flag = 0;
        }
        exit(1);
    }
    for(int i = 0; i < 100000; i++){
        while (sharedData->flag != 0);
        sharedData->num1--;
        sharedData->num2++;
        sharedData->flag = 1;
    }

    wait(NULL);

    printf("Result NUM1: %d || NUM2: %d\n", sharedData->num1, sharedData->num2);


    if( munmap(sharedData, sizeof(shared_data)) == -1 ){
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    if(close(fd) == -1){
        perror("close failed");
        exit(EXIT_FAILURE);
    }

    if(shm_unlink("/ex2") == -1){
        perror("unlink failed");
        exit(EXIT_FAILURE);
    }

    return 0;

}