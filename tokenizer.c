#include <stdio.h>
#include <string.h>


int main(int argc, char* argv[]){
    char *delim = " \t\n";

    for(char *token = strtok(argv, delim); token != NULL; token = strtok(NULL, delim)){
        
    }
}