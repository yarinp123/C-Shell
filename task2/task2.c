#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>

int main (int argc , char* argv[]){
    int fd[2];
    int c1, c2;
    char* args1[3] = {"ls", "-l", 0};
    char* args2[4] = {"tail", "-n", "2", 0};
    pipe(fd);
    c1 = fork();
    if(c1 == 0){
        close(1); //close standard output
        dup(fd[1]); //z`reopens a duplicate of the file in the first available fd (in this case, 1 because it's closed)
        close(fd[1]);
        execvp("ls", args1);
        perror("error in execvp command");
        exit(-1);
    }
    close(fd[1]);
    c2 = fork();
    if(c2 == 0){
        close(0); //close standard input
        dup(fd[0]);//reopens a duplicate of the file in the first available fd (in this case, 0 because it's closed)
        close(fd[0]);
        execvp("tail", args2);
        perror("error in execvp command");
        exit(-1);
    }
    close(fd[0]);
    waitpid(c1, NULL, 0);
    waitpid(c2, NULL, 0);
    return 1;
}