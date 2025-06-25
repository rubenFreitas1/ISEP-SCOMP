#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define RANGE 5

int main(){
    double money = 25;
    while(money > 0){
        printf("\n------ AVAILABLE MONEY: %.2f ------\n", money);

        int selectedNumber;
        double betMoney;
        int expectedNumber;
        int status;

        do{
        printf("Money to bet: ");
        scanf("%lf", &betMoney);
        }while(betMoney > money || betMoney <= 0);

        money -= betMoney;

        do{
        printf("Choose a number between 1-5: ");
        scanf("%d", &selectedNumber);
        }while(selectedNumber <= 0 || selectedNumber > 5);

        pid_t p = fork();
        if(p == -1){
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if(p == 0){
            execlp("./build/cheat", "cheat", NULL);
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
        
        waitpid(p, &status, 0);
        if(WIFEXITED(status)){
            expectedNumber = WEXITSTATUS(status);
            printf("\n------ RANDOM NUMBER: %d ------\n", expectedNumber);
            if(expectedNumber == selectedNumber){
                money += betMoney*2;
                printf("You guessed correctly! You won %.2f euros!\n", betMoney * 2);
            }else{
                printf("Wrong guess. You lost %.2f euros.\n", betMoney);
            }
        }else{
            printf("Cheat program ERROR.\n");
        }
    }
    return 0;
}