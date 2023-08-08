#include "LineParser.h"
#include "linux/limits.h"
#include "stdbool.h"
#include <fcntl.h>
#include "stdio.h"

bool isDebugMode = false;
void execute(cmdLine* pCmdLine){
    char* cmd = pCmdLine->arguments[0];
    if(strcmp("cd", cmd) == 0){
        if(chdir(pCmdLine->arguments[1]) == -1){
            fprintf(stderr, "cd error\n"); 
        }
    }
    else {     
        int pId = fork();
        if(!pId){
            if(pCmdLine->outputRedirect){
                close(1);
                int fd = open(pCmdLine->outputRedirect, O_CREAT | O_WRONLY );
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

            int status = execvp(cmd, pCmdLine->arguments);  /*child process*/
            if(status < 0){
                perror("Error )");
                exit(1);
            }
            exit(0);
        } 
        else 
        {
            if(pCmdLine->blocking){
                waitpid(pId, NULL, 0); /*Waiting*/
            }
            if(isDebugMode){
                fprintf(stderr, "Process: %d\n", pId);
                fprintf(stderr, "Executing command: %s\n", cmd);
            }
        }
    }
    freeCmdLines(pCmdLine);
}

int main (int argc , char* argv[], char* envp[]){
    char input[2048];
    char pathBuffer [PATH_MAX];
    cmdLine* nextCmd;
    for(int i = 1 ; i < argc ; i++){
        /*Searching for -D (Debug Mode) */
        isDebugMode = strncmp(argv[i], "-d", 2) == 0;
    }
    while (true){
        getcwd(pathBuffer, PATH_MAX);
        printf("%s: ",pathBuffer);
        fgets(input, 2048, stdin);
        if(strcmp(input, "quit\n") == 0 || feof(stdin))
            exit(0);
        nextCmd = parseCmdLines(input);
        execute(nextCmd);
    }
  return 0;
}
