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
#include <semaphore.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];

int is_prime(int n) {
    if (n <= 1) return 0; 
    for (int i = 2; i < n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}


void* thread_function(void *arg){
    int number = *((int*)arg);
    buffer[0] = '\0';
    
    for(int i = 2; i <= number; i++){
        if(is_prime(i)){
            char temp[32];
            sprintf(temp, "%d ", i);
            strncat(buffer, temp, BUFFER_SIZE - strlen(buffer) - 1);
        }
    }

    pthread_exit((void*)NULL);
}

int main(){
    pthread_t thread;
    int number;

    do{
        printf("Enter a positive Number: ");
        scanf("%d", &number);
    }while(number < 0);
    
    

    if( pthread_create(&thread, NULL, thread_function, (void*)&number) == -1){
        perror("pthread create failed");
        exit(EXIT_FAILURE);
    }


    pthread_join(thread, NULL);
    printf("Prime numbers up to %d:\n%s\n", number, buffer);

    return 0;
}