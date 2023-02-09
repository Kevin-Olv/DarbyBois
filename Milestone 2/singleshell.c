#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    char input[512];
    printf("myshell> ");        //print the prompt
    fgets(input,512,stdin);     //Get user input
    // printf("You entered: %s", input); //check for the right input 

    int rc = fork(); //create the fork
    if (rc < 0){
        // fork failed
        fprintf(stderr,"Fork Failed\n");
        exit(1);
    }
    else if (rc == 0){
        //child process
        // printf("Child Process PID:%d \n", (int) getpid());
    }
    else {
        //parent process
        // printf("Parent of %d, my pid: %d \n", rc, (int) getpid());
        
    }

    return 0;
}