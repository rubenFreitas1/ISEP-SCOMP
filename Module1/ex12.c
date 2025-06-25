#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) Dá print porque o execl falha.

int main(){
    printf ("Try to execute lls\n");
    execl("/bin/lls", "lls", NULL);
    printf("execl returned! errno is [%d]\n", errno);
    perror("The error message is: ");
    return 0;
}