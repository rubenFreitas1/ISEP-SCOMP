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
     for (int i = 0; i < 10; i++) {
        printf("teste1 running... %d\n", i);
        sleep(1);
    }
    return 0;
}