#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

typedef struct{
    char name[200];
    int number;
}Person;

int main(){
    pid_t pid;
    int fd[2];
    if(pipe(fd) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid == -1){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if(pid == 0){
        close(fd[1]);
        Person person;
        read(fd[0], &person, sizeof(Person));
        printf("NAME: %s || NUMBER: %d \n", person.name, person.number);
        close(fd[0]);
        exit(0);
    }
    Person person;
    close(fd[0]);
    char name[200];
    int number;
    printf("Name: ");
    scanf("%s", person.name);
    printf("Number: ");
    scanf("%d", &person.number);
    write(fd[1], &person, sizeof(Person));
    close(fd[1]);

    wait(NULL);
    return 0;

}