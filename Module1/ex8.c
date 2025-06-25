#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) A variável "a" vai ser igual nos dois processos, "b" do processo pai e "c" do processo filho são iguais, "d" do processo filho será igual 
//  ao "c" do processo pai.

int main(){
    int a,b,c,d;
    a= 0;
    b = fork();
    c = getpid();
    d = getppid();
    a = a+5;
    printf("\na=%d, b=%d, c=%d, d=%d\n", a,b,c,d);
}