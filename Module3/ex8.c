#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

typedef struct {
    int customer_code;
    int product_code;
    int quantity;
} record_t;

#define RECORDS 500
#define RECORDS_PER_PROCESS 50
#define PROCESSES 10


int main(){
    pid_t pids[PROCESSES];
    int fd[2];
    record_t sales[RECORDS];
    int larger[RECORDS];
    int bytes_read = 0;
    if(pipe(fd) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    int product_codes[RECORDS];
    for(int i = 0; i < RECORDS; i++) {
        product_codes[i] = i + 1; // Ex: 1..500
    }

    for(int i = 0; i < RECORDS; i++){
        sales[i].customer_code = rand() % 1000;
        sales[i].product_code = product_codes[i];
        sales[i].quantity = rand() % 22;
    }

    for(int i = 0; i < PROCESSES; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
        if(pids[i] == 0){
            close(fd[0]);   

            int start = i*RECORDS_PER_PROCESS;
            int end = start + RECORDS_PER_PROCESS;

            for(int w = start; w < end; w++){
                if(sales[w].quantity > 20 ){
                    write(fd[1], &sales[w].product_code, sizeof(int));
                }
            } 

            close(fd[1]);
            exit(EXIT_SUCCESS);
        }
        
    }

    close(fd[1]);

    int count = 0;
    int product_code;

    while( (bytes_read = read(fd[0], &product_code, sizeof(int))) > 0){
        larger[count] = product_code;
        count++;
    }

    close(fd[0]);

    for(int i = 0 ; i < PROCESSES; i++){
        waitpid(pids[i], NULL, 0);
    }

    printf("Product codes with quantity > 20:\n");
    for(int i = 0; i < count; i++){
        printf("%d ", larger[i]);
    }
    printf("\n");
    
    return 0;
}