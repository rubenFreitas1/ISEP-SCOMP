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

void* thread_function(void *arg){
    char *str = (char*) arg;
    write(STDOUT_FILENO, str, strlen(str));
    pthread_exit((void*)NULL);
}


int main(){

    pthread_t thread1, thread2;
    char str[] = "Rúben ";
    char str1[] = "Freitas\n";

    if( (pthread_create(&thread1, NULL, thread_function, (void*) str)) == -1){
        perror("pthread create failed");
        exit(EXIT_FAILURE);
    }

    if((pthread_create(&thread2, NULL, thread_function, (void*)str1)) == -1){
        perror("pthread create failed");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_exit(NULL);
}