#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) Foram criados 7 processos novos (sem contar com o processo inicial). No final são 8 processos.

int main(){
    fork();
    printf("1\n");
    fork();
    printf("2\n");
    fork();
    printf("3\n");
    return 0;
}