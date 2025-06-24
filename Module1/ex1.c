#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) 3 processos foram criados (sem contar com o processo pai).
//b) 4 vezes.



int main(){
    pid_t p, a;
    p = fork();
    if(p == -1){
		perror("fork:");
		exit(1);
	}
    a = fork();
    if(a == -1){
		perror("fork:");
		exit(1);
	}
    printf("Concurrent Programming\n");
    return 0;
}