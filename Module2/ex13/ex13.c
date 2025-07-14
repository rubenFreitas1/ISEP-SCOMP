#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define FILENAME "data.txt"
#define MAX_CYCLES 10

volatile sig_atomic_t sigusr1_flag = 0;
volatile sig_atomic_t sigusr2_flag = 0;
volatile sig_atomic_t sigint_flag = 0;

void sigusr1_handler(int sig) {
    sigusr1_flag = 1;
}

void sigusr2_handler(int sig) {
    sigusr2_flag = 1;
}

void sigint_handler(int sig) {
    sigint_flag = 1;
}

void setup_sigmask() {
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);
}


int main() {
    pid_t pid;
    srand(getpid());

    struct sigaction sa_usr1, sa_usr2, sa_int;
    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;

    sa_usr2.sa_handler = sigusr2_handler;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;

    sigaction(SIGUSR1, &sa_usr1, NULL);
    sigaction(SIGUSR2, &sa_usr2, NULL);
    sigaction(SIGINT, &sa_int, NULL);

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    setup_sigmask(); // Bloquea sinais excepto SIGUSR1 e SIGUSR2

    if (pid == 0) {
        int cycles = 0;
        while (cycles < MAX_CYCLES) {
            while (!sigusr2_flag && !sigint_flag) pause();

            if (sigint_flag) {
                printf("Filho: SIGINT recebido, a terminar.\n");
                exit(0);
            }

            sigusr2_flag = 0;

            FILE *f = fopen(FILENAME, "w");
            if (!f) {
                perror("filho fopen");
                exit(EXIT_FAILURE);
            }

            int novo_num = rand() % 1000;
            fprintf(f, "%d\n", novo_num);
            fclose(f);

            printf("Filho: escreveu %d em %s\n", novo_num, FILENAME);

            kill(getppid(), SIGUSR1);
            cycles++;
        }
    } else {
        FILE *f = fopen(FILENAME, "w");
        if (!f) {
            perror("pai fopen");
            exit(EXIT_FAILURE);
        }
        fprintf(f, "0\n");
        fclose(f);

        kill(pid, SIGUSR2);

        int cycles = 0;
        while (cycles < MAX_CYCLES) {
            while (!sigusr1_flag && !sigint_flag) pause();

            if (sigint_flag) {
                printf("Pai: SIGINT recebido, a terminar.\n");
                kill(pid, SIGKILL);
                wait(NULL);
                exit(0);
            }

            sigusr1_flag = 0;

            FILE *f = fopen(FILENAME, "r");
            if (!f) {
                perror("pai fopen");
                kill(pid, SIGKILL);
                wait(NULL);
                exit(EXIT_FAILURE);
            }

            int num;
            fscanf(f, "%d", &num);
            fclose(f);

            printf("Pai: leu %d de %s\n", num, FILENAME);

            kill(pid, SIGUSR2);
            cycles++;
        }

        printf("Pai: ciclos concluídos. A terminar.\n");
        kill(pid, SIGKILL);
        wait(NULL);
        exit(0);
    }

    return 0;
}