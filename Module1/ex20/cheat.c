#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>


int main(){
    srand(time(NULL));
    int expectedNumber = (rand() % 5) + 1;
    exit(expectedNumber);
}