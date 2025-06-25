#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>

//a) Dá print 8 vezes em exam pois ao fazer execlp o código é substituido pelo exam.c, ou seja, só faz uma iteração do loop e nunca
// chega a fazer execlp(student).

int main(){
    pid_t pid;
	pid = fork();
	pid = fork();
	for(int i = 0; i<5; i++){
		pid = fork();
		
		execlp("./build/exam", "exam", NULL);
		
		if(pid == 0){
			break;
		}
	}
	execlp("./build/students", "students", "scomp", NULL);
    return 0;
}