#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(){
    execlp("./script.sh", "./script.sh", NULL); 
    perror("exec falhou");
    exit(EXIT_FAILURE);
}