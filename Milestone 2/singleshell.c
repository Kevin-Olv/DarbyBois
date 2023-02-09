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

    return 0;
}