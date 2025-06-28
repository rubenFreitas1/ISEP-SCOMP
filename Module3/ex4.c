#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

int main(){
    int down[2], up[2];
    pid_t pid;
    char buffer[500];
    int bytes_read;
    if(pipe(down) == -1 || pipe(up) == -1){
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if(pid == -1){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(pid == 0){
        close(up[0]);
        close(down[1]);
        printf("Message: ");
		fgets(buffer, sizeof(buffer), stdin);
        write(up[1], buffer, sizeof(buffer));
        bytes_read = read(down[0], buffer, sizeof(buffer));
        if(bytes_read > 0){
            buffer[bytes_read] = '\0';
            printf("Server tranformed your big quote into: %s", buffer);
        }
        close(up[1]);
        close(down[0]);
        exit(EXIT_SUCCESS);
    }

    close(up[1]);
	close(down[0]);
    bytes_read = read(up[0], buffer, sizeof(buffer));
    if(bytes_read > 0){
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] >= 97 && buffer[i] <= 122) {
                buffer[i] = buffer[i] - 32;
            } else if (buffer[i] >= 65 && buffer[i] <= 90) {
                buffer[i] = buffer[i] + 32;
            }
        }
        write(down[1], buffer, sizeof(buffer));	
    }
    close(down[0]);
    close(up[1]);
    wait(NULL);
    return 0;
}
