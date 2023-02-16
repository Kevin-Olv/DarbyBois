#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define MAX_LENGTH 100

char g_bKeepLooking = 1;

// Function to Capture the Signals
void signalCapture(int nSigNum){
    //printf("Signal %d has been captured\n", nSigNum);
    if(nSigNum == SIGINT){
        printf("\nControl-C was pressed ... exiting\n");
        g_bKeepLooking = 0;
    }
}

//Help function
void help(){
    printf("Here is a list of commands: \nexit\nstart\nrun\nwait\nwaitfor\nkill\nmyshell> ");
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
    //
    char * words[MAX_LENGTH];
    
    // char array to store input of max size
    char input[1024];

    while(fgets(input,1024,stdin) != NULL){ //While Loop to keep running unless it gets an E.O.F or NULL

        //Break input into seperate words
        words[0] = strtok(input," \t\n");
        for(int i=1; i < MAX_LENGTH; i++){
            //printf("%s\n",words[i-1]);
            words[i] = strtok(0," \t\n");

            //keep track of the number of arguments passed in
            nwords=i;
            //Break the Loop if Null is entered
            if(words[i]==NULL){
                break;
            }
        }

        words[nwords] = 0;
        //Begin Strcmp to see what the argument is 
        if(strcmp(words[0],"exit")==0){
            //here put the exit function
            printf("Recieved Exit command\n");
            break;
        }
        //next case
        else if(strcmp(words[0],"start")==0){
            //here begins the start function

            //check for atleast 2 arguments to start func
            if(nwords<2){
                printf("Not enough arguments for start function\nmyshell> ");
            }
            //create the fork
            pid_t pid = fork();

            if(pid<0){
                //Fork Failed
                printf("Start failed: Failed forking child...\nmyshell> ");
            }
            else if(pid==0){
                //Child Process

                //Execvp call: If <0 then it failed so 
                if(execvp(words[1],&words[1])<0){
                    printf("Start: exec failed: %s\n",strerror(errno));
                }
            }
            else if(pid > 0){
                //Parent process
                printf("ndshell: Process %d started\n",pid);
                wait(NULL);
                printf("ndshell> ");
            }
        }
        else if(strcmp(words[0],"wait")==0){
            //here begins the wait function

            pid_t pid;
            int stat;
            //By doing waitpid(-1) we wait for any Child process to finish
            pid = waitpid(-1, &stat, WNOHANG);

            if(WIFEXITED(stat)){
                //WIFEXITED returns true if child exited normally
                //WEXITSTATUS returns the exit status of the child
                printf("ndshell: process %d exited normally with status %d\nmyshell> ",pid,WEXITSTATUS(stat));
                //Exit status: 0 is successful 

            }
            //WIFSIGNALED returns true if terminated by a signal
            else if(WIFSIGNALED(stat)){
                //WTERMSIG returns the signal number that caused child to terminate
                //psignal displays a message on standard error describing the signal
                psignal(WTERMSIG(stat),"Exit Signal");
            }
        }
        else if(strcmp(words[0],"waitfor")==0){
            //here begins the waitfor function

            //Check to see if the right number of arguments are passed
            if(nwords!=2){
                printf("Incorrect number of arguments to waitfor\nmyshell> ");
                continue;
            }
            
            pid_t pid;
            pid_t wpid;
            //here we convert the pid inputted thats a string to an integer
            wpid = atoi(words[1]);
            int stat;
            //wait pid makes system wait for a specific process we want
            pid = waitpid(wpid, &stat, WNOHANG);

            //If waitpid returns < 0 then an error occured 
            if(pid < 0){
                printf("Exit status: %d %s\nmyshell> ", WEXITSTATUS(stat),strsignal(WTERMSIG(stat)));
            }
            //WIFSIGNALED returns true if terminated by a signal
            else if(WIFSIGNALED(stat)){
                //WTERMSIG returns the signal number that caused child to terminate
                //psignal displays a message on standard error describing the signal
                psignal(WTERMSIG(stat),"Exit Signal");
            }

        }
        else if(strcmp(words[0],"run")==0){
            //here begins the run function (combine start + waitfor)

            //check for atleast 2 arguments to start func
            if(nwords<2){
                printf("Not enough arguments for start function\nmyshell> ");
            }
            //create the fork
            pid_t pid = fork();

            if(pid<0){
                //Fork Failed
                printf("Start failed: Failed forking child...\nmyshell> ");
            }
            else if(pid==0){
                //Child Process

                //Execvp call: If <0 then it failed so 
                if(execvp(words[1],&words[1])<0){
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
            pid = waitpid(pid, &stat, 0);
            if(pid < 0){
                printf("Exit Status: %d %s\n",pid,strsignal(WTERMSIG(stat)));

            }
            else if(WIFSIGNALED(stat)){
                psignal(WTERMSIG(stat),"Exit Signal");
            }
        }
        else if(strcmp(words[0],"kill")==0){
            //here begins the kill function
        }
        else if(strcmp(words[0],"help")==0){
            //here begins the help function
            help();
        }
        else{
            printf("myshell> Unknown Command: %s\n", words[0]);
            //insert the help function here
            help();
        }
        

    }



    return 0;
}