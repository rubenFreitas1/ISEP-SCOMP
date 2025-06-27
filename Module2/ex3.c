#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

void handle_signal(int sig){
    const char msg[] = "I got signal SIGSEGV\n";
	write(STDOUT_FILENO, msg, sizeof(msg)-1);
	exit(1);
}

int main(){
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = handle_signal;
    act.sa_flags = SA_RESTART;
    sigaction(SIGSEGV, &act, NULL);

    int a;
    a = *(int *)0;
}