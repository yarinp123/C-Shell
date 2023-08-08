#include "string.h"
#include "sys/wait.h"
#include <fcntl.h>
#include "stdlib.h"
#include "unistd.h"
#include "LineParser.h"
#include "stdio.h"
#include "errno.h"
#include "linux/limits.h"
#include "stdbool.h"
#define HISLENGTH 3
bool isDebugMode = false;

void handlePipeLine(cmdLine* pCmdLine){
    int fd[2];
    int c1, c2;
    pipe(fd);
    c1 = fork();
    if(c1 == 0){

        if(pCmdLine->outputRedirect){
            close(1);
            int fd = open(pCmdLine->outputRedirect, O_CREAT | O_WRONLY);
            if(fd < 0){
                perror("file open error");
                exit(-1);
            }
        }    

        if(pCmdLine->inputRedirect){
            close(0);
            int fd = open(pCmdLine->inputRedirect, O_RDONLY);
            if(fd < 0){
                perror("file open error");
                exit(-1);
            }
        }

        close(1);
        dup(fd[1]);
        close(fd[1]);
        execvp(pCmdLine->arguments[0], pCmdLine->arguments);
        perror("error in execvp command");
        exit(-1);
    }

    close(fd[1]);
    c2 = fork();
    if(c2 == 0){

        if(pCmdLine->next->outputRedirect){
            close(1);
            int fd = open(pCmdLine->next->outputRedirect,  O_CREAT | O_WRONLY );
            if(fd < 0){
                perror("file open error");
                exit(-1);
            }
        if(pCmdLine->next->inputRedirect){
            close(0);
            int fd = open(pCmdLine->next->inputRedirect, O_RDONLY);
            if(fd < 0){
                perror("file open error");
                exit(-1);
            }
        }

        }
                
        close(0);
        dup(fd[0]);
        close(fd[0]);
        execvp(pCmdLine->next->arguments[0], pCmdLine->next->arguments);
        perror("error in execvp command");
        exit(-1);
    }
    close(fd[0]);
    waitpid(c1, NULL, 0);
    waitpid(c2, NULL, 0);
}

void execute(cmdLine* pCmdLine){
    char* command = pCmdLine->arguments[0];
    if(strcmp(command, "cd") == 0){
        if(chdir(pCmdLine->arguments[1]) == -1){
            fprintf(stderr, "cd error!\n");
        }
    }else {
        if(pCmdLine->next){
            handlePipeLine(pCmdLine);
        } else {
            int processId = fork();
            if(!processId){
                
                if(pCmdLine->outputRedirect){
                    close(1);
                    int fd = open(pCmdLine->outputRedirect, O_CREAT | O_WRONLY);
                    if(fd < 0){
                        perror("open");
                        exit(-1);
                    }
                }
                if(pCmdLine->inputRedirect){
                    close(0);
                    int fd = open(pCmdLine->inputRedirect, O_RDONLY);
                    if(fd < 0){
                        perror("open");
                        exit(-1);
                    }
                }

                int status = execvp(command, pCmdLine->arguments);
                if(status < 0){
                    perror("execvp err!");
                    exit(1);
                }
                exit(0);
            } else {
                if(pCmdLine->blocking){
                    waitpid(processId, NULL, 0);

                }
                if(isDebugMode){
                    fprintf(stderr, "process id: %d\n", processId);
                    fprintf(stderr, "command onging: %s\n", command);
                }
            }
        }
    }
    freeCmdLines(pCmdLine);
}


int main (int argc , char* argv[], char* envp[]){
    char history[HISLENGTH][2048];
    char input[2048];
    char pathBuffer[PATH_MAX];
    int newIdx = 0;
    int oldIdx = 0;
    bool isCircular = false;
    cmdLine* nextCmd;
    int size = 0;
    for(int i = 1 ; i < argc ; i++){
        /*Searching for -D (Debug Mode) */
        isDebugMode = strncmp(argv[i], "-d", 2) == 0;
    }
    while (true){
        getcwd(pathBuffer, PATH_MAX);
        printf("%s: ",pathBuffer);
        fgets(input, 2048, stdin);
        if(strcmp(input, "quit\n") == 0 || feof(stdin) ) {
            exit(0);
        }
        if(strcmp(input, "history\n") == 0){
            for(int i = 0; i < size; i++){
                printf("%d) %s", i+1, history[(i + oldIdx)%HISLENGTH]);
            }
        } else
        if(strcmp(input, "!!\n") == 0){
            if(newIdx != 0)
                nextCmd = parseCmdLines(history[newIdx-1]);
            else 
                nextCmd = parseCmdLines(history[HISLENGTH-1]);
            execute(nextCmd);
        } else
        if(strncmp(input, "!", 1) == 0){
            int index = atoi(input+1);
            if(index <= size){
                nextCmd = parseCmdLines(history[(index + oldIdx - 1)%HISLENGTH]);
                execute(nextCmd);
            } else {
                fprintf(stderr, "out of bound\n");
            }
        } else {
            strcpy(history[newIdx], input);
            newIdx++;
            if(isCircular){
                oldIdx++;
            } else {
                size++;
            }
            if(newIdx == HISLENGTH){
                newIdx = 0;
                isCircular = true;
            }
            nextCmd = parseCmdLines(input);
            execute(nextCmd);
        }
    }
  return 0;
}
