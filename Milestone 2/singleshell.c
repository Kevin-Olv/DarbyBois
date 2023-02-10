#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

char g_bKeepLooking = 1;

// Function to Capture the Signals
void signalCapture(int nSigNum){
    //printf("Signal %d has been captured\n", nSigNum);
    if(nSigNum == SIGINT){
        printf("\nControl-C was pressed ... exiting\n");
        g_bKeepLooking = 0;
    }
}

int main(int argc, char *argv[]){
    
    struct sigaction sigSetValue;
    /* Define the struct */
    sigSetValue.sa_handler = signalCapture;
    sigaction(SIGHUP,  &sigSetValue, NULL);
    sigaction(SIGINT,  &sigSetValue, NULL);
    sigaction(SIGKILL, &sigSetValue, NULL);
    sigaction(SIGTERM, &sigSetValue, NULL);
    sigaction(SIGQUIT, &sigSetValue, NULL);
    sigaction(SIGUSR1, &sigSetValue, NULL);
          
    // Begin Loop
    while(g_bKeepLooking)
    {
    char input[100];
    printf("myshell> ");        //print the prompt
    fgets(input,100,stdin);     //Get user input
    char theCopy[100];

    strncpy(theCopy,input,strlen(input)-1);    //Copy the input string into theCopy char array
    theCopy[strlen(input)-1] = '\0';           // But make sure the last thing is NULL char not \n

    // Char array to pass to the child process for exec
    char * myargs[2];
    myargs[0] = strdup(theCopy);
    myargs[1] = NULL; 

    int rc = fork(); //create the fork
    if (rc < 0){
        // fork failed
        fprintf(stderr,"Fork Failed\n");
        exit(1);
    }
    else if (rc == 0){
        //child process
        
        if((execvp(myargs[0], myargs)) < 0){
            //Error Checking
            printf("exec failed: %s\n",strerror(errno));
            //Exit Flag for the loop
            g_bKeepLooking = 0;
        }
    }
    else {
        //parent process
        int wc = wait(NULL);
        g_bKeepLooking = 0;
        printf("Execution Complete wc:%d \n", wc);
    }
     
     
     sleep(1);    
    }

    return 0;
}