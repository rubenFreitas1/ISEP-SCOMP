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
    struct sigaction sa;
    sa.sa_handler = SIG_IGN; // Ignorar o sinal
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        char sentence[256];
        printf("Enter a sentence: ");
        fflush(stdout);
        if (fgets(sentence, sizeof(sentence), stdin) != NULL) {
            printf("You entered: %s", sentence);
        }
        exit(0);
    }

    sleep(10);
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        printf("\nYou are too slow!\n");
        kill(pid, SIGKILL);
        waitpid(pid, NULL, 0);
    }
    return 0;
}