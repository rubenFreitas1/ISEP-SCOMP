#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STR_SIZE 50
#define NR_COURSES 10
#define STUDENTS 5


typedef struct{
    int done1;
    int done2;
    int ready;
    int number;
    char name[STR_SIZE];
    int courses[NR_COURSES];
}student_t;


int main(){
    int fd;
    student_t *sharedData;
    pid_t pids[2];

    if( (fd = shm_open("/ex8", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1){
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if(ftruncate(fd, sizeof(student_t)) == -1){
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    if( (sharedData = (student_t*)mmap(sharedData, sizeof(student_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED){
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    
    sharedData->ready = 0;
    sharedData->done1 = 1;
    sharedData->done2 = 1;
    
    for(int i = 0; i < 2; i++){
        pids[i] = fork();
        if(pids[i] == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if(pids[i] == 0){
            if(i == 0){
                for(int j = 0; j < STUDENTS; j++){
                    while(sharedData->ready == 0);
                    int higher = sharedData->courses[0];
                    int lower = sharedData->courses[0];
                    for(int w = 0; w < NR_COURSES; w++){
                        if(sharedData->courses[w] > higher){
                            higher = sharedData->courses[w];
                        }
                        if(sharedData->courses[w] < lower){
                            lower = sharedData->courses[w];
                        }
                    }
                    printf("HIGHEST GRADE: %d || LOWEST GRADE: %d \n", higher, lower);
                    sharedData->done1 = 1;
                    while(sharedData->ready == 1);
                }
                exit(1);
                
            }else if(i == 1){
                for(int j = 0; j < STUDENTS; j++){
                    while(sharedData->ready == 0);
                    float average = 0;
                    for(int w = 0; w < NR_COURSES; w++){
                        average += (float) sharedData->courses[w];
                    }
                    average = (float) average / (float) NR_COURSES;
                    printf("AVERAGE GRADE: %.2f \n", average);
                    sharedData->done2 = 1;
                    while(sharedData->ready == 1);
                }
                exit(1);
            }
        }
    }
 
    for(int z = 0; z < STUDENTS; z++){
        while (!(sharedData->done1 == 1 && sharedData->done2 == 1 && sharedData->ready == 0));	
        printf("Number: ");
	    scanf("%d", &sharedData->number);

	    printf("Name: ");
	    scanf("%s", sharedData->name);

	    printf("Enter grades for %d courses:\n", NR_COURSES);
	    for (int i = 0; i < NR_COURSES; i++) {
		    printf("Course %d grade: ", i + 1);
		    scanf("%d", &sharedData->courses[i]);
	    }

	    sharedData->done1 = 0;
        sharedData->done2 = 0;
        sharedData->ready = 1;
		while(sharedData->done1 == 0 && sharedData->done2 == 0);
		sharedData->ready = 0;
    }

    for(int i = 0; i< 2; i++){
        wait(NULL);
    }

    if(munmap(sharedData, sizeof(student_t)) == -1){
        perror("munmap failed");
        exit(EXIT_FAILURE);
    }

    if(close(fd) == -1){
        perror("close fd failed");
        exit(EXIT_FAILURE);
    }

    if(shm_unlink("/ex8") == -1){
        perror("unlink failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}