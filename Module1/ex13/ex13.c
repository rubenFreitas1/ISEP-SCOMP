#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) Dá print 6 vezes em scomp. 

int main(){
    pid_t pid;
	for(int i = 0; i<3; i++){
		pid = fork();
		if(pid == 0){
			pid = fork();
		}
		
		if(pid > 0){
			execlp("./build/scomp", "scomp", NULL);
		}
	}

    return 0;
}