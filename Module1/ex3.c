#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) Foram criados 15 processos novos.
//b) Dá print 16 vezes.

int main(){
    pid_t pid;
    int i;

    for(i = 0; i < 4; i++){
        pid = fork();
    }

    printf ("Concurrent Programming \n" );
    return 0;
}