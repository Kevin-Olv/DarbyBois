#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>

#define MAX_LENGTH 100

char g_bKeepLooking = 1;


void signalCapture(int nSigNum){ // Function to Capture the Signals
    
    if(nSigNum == SIGINT){
        printf("\nControl-C was pressed ... exiting\n");
        g_bKeepLooking = 0;
        exit(0);
    }
}

void help(){ //Help function
    printf("Here is a list of commands: \nexit\nstart\nrun\nwait\nwaitfor\nkill\nquit\nndshell> ");
}

void kill_cmd(int pid) { //kill function
    
    int status;
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        printf("process %d exited with status %d\n", pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("process %d exited abnormally with signal %d: Killed.\nndshell> ", pid, WTERMSIG(status));
    }
}

void quit_cmd() { //quit function
    int status;
    pid_t pid;

    /* send SIGINT to all child processes */
    kill(0, SIGQUIT);

    /* wait for all child processes to exit */
    while ((pid = waitpid(-1, &status, 0)) > 0) {
        printf("ndshell: process %d exited abnormally with signal %d: Killed.\n", pid, status);
    }

    printf("All child processes complete – exiting the shell.\n");
    exit(EXIT_SUCCESS);
}

void wait_cmd() { //wait function
    
     pid_t pid;
    int stat;

    //By doing waitpid(-1) we wait for any Child process to finish
    pid = waitpid(-1, &stat, WNOHANG);

    if(pid <= 0){
        printf("ndshell: No children\nndshell> ");
    }

    else if(WIFEXITED(stat)){
        //WIFEXITED returns true if child exited normally
        //WEXITSTATUS returns the exit status of the child
        printf("ndshell: process %d exited normally with status %d\nndshell> ", pid, WEXITSTATUS(stat));
                

    }
    //WIFSIGNALED returns true if terminated by a signal
    else if(WIFSIGNALED(stat)){
        //WTERMSIG returns the signal number that caused child to terminate
        printf("ndshell: process %d exited abnormally with signal %d: \nndshell> ", pid, WTERMSIG(stat));
                
    }
}

void waitfor_cmd(int wpid){

    int stat;
    //int temppid = pid;
    pid_t pid;
    //wait pid makes system wait for a specific process we want
    pid = waitpid(wpid, &stat, WNOHANG);

    //If waitpid returns < 0 then an error occured 
    if(pid < 0){
        printf("ndshell: no such process\nndshell> ");
    }
    else {
        printf("ndshell: process %d exited normally with status %d\nndshell> ", wpid, WEXITSTATUS(stat));
    }
}

void run_cmd(char * words[]){ //run function

    pid_t pid = fork();

    if(pid < 0){
        //Fork Failed
        printf("Start failed: Failed forking child...\nndshell> ");
    }
    else if(pid == 0){
        //Child Process

        //Execvp call: If <0 then it failed so 
        if(execvp(words[1], &words[1]) < 0){
            printf("Start: exec failed: %s\n",strerror(errno));
        }
    }
    else if(pid > 0){
        //Parent process
        printf("ndshell: Process %d started\n",pid);
        wait(NULL);
        printf("ndshell> ");
    }

    int stat;
    int temppid = pid;
    pid = waitpid(pid, &stat, 0);
    if(pid < 0){
        printf("process %d exited normally with status %d\nndshell> ", temppid, WEXITSTATUS(stat));

    }
    else if(WIFSIGNALED(stat)){
        psignal(WTERMSIG(stat),"Exit Signal");
    }
}

void start_cmd(char * words[]) {
    //create the fork
    pid_t pid = fork();

    if(pid < 0){
        //Fork Failed
        printf("Start failed: Failed forking child...\nndshell> ");
    }
    else if(pid == 0){
        //Child Process

        //Execvp call: If <0 then it failed so 
        if(execvp(words[1], &words[1]) < 0){
            printf("Start: exec failed: %s\n", strerror(errno));
        }
    }
    else if(pid > 0){
        //Parent process
        printf("ndshell: Process %d started\n", pid);
        sleep(1);
        printf("ndshell> ");
    }
}

int main(int argc, char * argv[]){
    
    // Signal Handler Code
    struct sigaction sigSetValue;
    /* Define the struct */
    sigSetValue.sa_handler = signalCapture;
    sigaction(SIGHUP,  &sigSetValue, NULL);
    sigaction(SIGINT,  &sigSetValue, NULL);
    sigaction(SIGKILL, &sigSetValue, NULL);
    sigaction(SIGTERM, &sigSetValue, NULL);
    sigaction(SIGQUIT, &sigSetValue, NULL);
    sigaction(SIGUSR1, &sigSetValue, NULL);


    //Shell Prompt
    printf("ndshell> ");

    //Number of words in the argument
    int nwords = 1;
    char * words[MAX_LENGTH];
    
    // char array to store input of max size
    char input[1024];

    while(fgets(input,1024,stdin) != NULL){ //While Loop to keep running unless it gets an E.O.F or NULL

        //Break input into seperate words
        words[0] = strtok(input," \t\n");
        for(int i=1; i < MAX_LENGTH; i++){
            
            words[i] = strtok(0," \t\n");
            nwords=i; //keep track of the number of arguments passed in

            //Break the Loop if Null is entered
            if(words[i] == NULL) {
                break;
            }
        }

        words[nwords] = 0;
        //Begin Strcmp to see what the argument is 
        if(strcmp(words[0],"exit")==0){ //here put the exit function
            printf("Exiting shell immediately\n");
            break;
        }
        
        else if(strcmp(words[0],"start") == 0){ //here begins the start function

            //check for atleast 2 arguments to start func
            if(nwords < 2){
                printf("Not enough arguments for start function\nndshell> ");
            }

            start_cmd(words);
            
        }
        else if(strcmp(words[0],"wait")==0){ //here begins the wait function
            wait_cmd();
        }
        else if(strcmp(words[0],"waitfor")==0){ //here begins the waitfor function

            //Check to see if the right number of arguments are passed
            if(nwords!=2){
                printf("Incorrect number of arguments to waitfor\nndshell> ");
                continue;
            }
            if(isdigit(*words[1])) {
                pid_t wpid;

                wpid = atoi(words[1]);
                waitfor_cmd(wpid);
            }
            else {
                printf("Error: Bad PID Non-numeric value \nndshell> ");
            }

        }
        else if(strcmp(words[0],"run")==0){ //here begins the run function (combine start + waitfor)

            //check for atleast 2 arguments to start func
            if(nwords<2){
                printf("Not enough arguments for start function\nndshell> ");
            }
            run_cmd(words);
          
        }
        else if(strcmp(words[0],"kill")==0){ //here begins the kill function
            if(nwords < 2){
                printf("Not enough arguments for start function\nndshell> ");
            }

            kill_cmd(atoi(words[1]));
        }
        else if(strcmp(words[0],"help")==0){ //here begins the help function
            help();
        }
        else if(strcmp(words[0], "quit") == 0){
            quit_cmd();
        }
        else{
            printf("ndshell> Unknown Command: %s\n", words[0]);
            //insert the help function here
            help();
        }
        

    }

    return 0;
}