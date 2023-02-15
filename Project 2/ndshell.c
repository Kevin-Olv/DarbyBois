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
            printf("%s\n",words[i-1]);
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
        if(strcmp(words[0],"exit")){
            //here put the exit function
            printf("Recieved Exit command\n");
            break;
        }
        //next case
        else if(strcmp(words[0],"start")){
            //here put the start function
        }
        else if(strcmp(words[0],"wait")){
            //here put the wait function
        }
        else if(strcmp(words[0],"waitfor")){
            //here put the waitfor function
        }
        else if(strcmp(words[0],"run")){
            //here put the run function (combine start + waitfor)
        }
        else if(strcmp(words[0],"kill")){
            //here put the kill function
        }
        else if(strcmp(words[0],"help")){
            //here put the help function
        }
        else{
            printf("myshell> Unknown Command: %s\n", words[0]);
            //insert the help function here
        }
        

    }



    return 0;
}