#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t woke_up = 0;

void handler(int sig) {
    woke_up = 1;
}

void my_sleep(unsigned int seconds) {
    struct sigaction sa;
    sigset_t new_mask, old_mask;

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    sigfillset(&new_mask);          // Bloqueia todos
    sigdelset(&new_mask, SIGALRM);  // Exceto SIGALRM
    sigprocmask(SIG_SETMASK, &new_mask, &old_mask);

    alarm(seconds);

    while (!woke_up)
        pause();

    sigprocmask(SIG_SETMASK, &old_mask, NULL);
}

int main(){
    int seconds;
    printf("Seconds to sleep: ");
    scanf("%d", &seconds);
    printf("Going to sleep for %d seconds...\n", seconds);
    my_sleep(seconds);
    printf("Woke up!\n");
    return 0;

}