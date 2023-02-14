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
    char * input[MAX_LENGTH];




    return 0;
}