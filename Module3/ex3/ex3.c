#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 1000

int main(){
    pid_t pid;
    int fd[2], bytes_read;
    FILE *file;
    char buffer[BUFFER_SIZE];

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
        bytes_read = read(fd[0], buffer, BUFFER_SIZE);
		if(bytes_read > 0){
			printf("Content from the file:\n %s \n", buffer);
		}
		close(fd[0]);
		exit(EXIT_SUCCESS);
    }
    close(fd[0]);
    file = fopen("teste.txt", "r");
	if(file == NULL){
		perror("file failed: ");
		exit(EXIT_FAILURE);
    }
    while( (bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0){
			write(fd[1], buffer, bytes_read);
	}
    close(fd[1]);
    fclose(file);

    wait(NULL);
    return 0;
}