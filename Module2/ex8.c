#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#define DB_SIZE 10000
#define CHILDS 10

typedef struct {
    char name[50];
    char region[50];
    int phone;
}record_t;

record_t db[DB_SIZE];
pid_t pids[CHILDS];

volatile sig_atomic_t found_flag = 0;

void sig_found(int sig) {
    found_flag = 1;
}

void populate_db() {
    for (int i = 0; i < DB_SIZE; i++) {
        snprintf(db[i].name, sizeof(db[i].name), "Workshop%d", i);
        snprintf(db[i].region, sizeof(db[i].region), "Region%d", i % 100); // 100 regiões diferentes
        db[i].phone = 100000 + i;
    }
}

int main(){
    char target_region[50];
    populate_db();
    printf("Enter region to search: ");
    scanf("%s", target_region);

    struct sigaction sa;
    sa.sa_handler = sig_found;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    int records_per_proc = DB_SIZE / CHILDS;

    for (int i = 0; i < CHILDS; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork failed");
            exit(1);
        }
        if (pids[i] == 0) {
            int start = i * records_per_proc;
            int end = start + records_per_proc;
            for (int j = start; j < end; j++) {
                if (strcmp(db[j].region, target_region) == 0) {
                    printf("Found: %s, Phone: %d (Process %d)\n", db[j].name, db[j].phone, getpid());
                    kill(getppid(), SIGUSR1); 
                    exit(0);
                }
            }
            exit(0); 
        }
    }

    int finished = 0;
    while (finished < CHILDS && !found_flag) {
        int status;
        pid_t pid = wait(&status);
        if (pid > 0) {
            finished++;
        }
    }

    if (found_flag) {
        for (int i = 0; i < CHILDS; i++) {
            if (kill(pids[i], 0) == 0) { 
                kill(pids[i], SIGKILL);
            }
        }
    }

     while (finished < CHILDS) {
        wait(NULL);
        finished++;
    }

    if (!found_flag) {
        printf("Region not found!\n");
    }

    printf("Search completed.\n");

    return 0;
}