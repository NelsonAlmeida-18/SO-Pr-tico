#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char* argv[]){
    if (argc<3){
        perror("Not enough arguments;");
        return 1;
    }
    else{
        int fi = open(argv[1],O_RDONLY, 0644);
        int fd = open(argv[2],O_CREAT, O_TRUNC, O_WRONLY, 0644);

        int status;

        int fds[2];
        if(pipe(fds)==-1){
            perror("An error has occured while setting a pipe;");
            return 1;
        }
        if (fork()==0){
            close(fds[0]);// não vamos ler nada da pipe por isso fechamos o descritor de leitura
            char* buffer;
            int bytesRead;
            while((bytesRead=read(fi, &buffer, 1024)>0)){
                write(fds[1],&buffer, bytesRead);
            }
            close(fds[1]);
            _exit(status);
        }

        wait(&status);
        if (WIFEXITED(status)){
            
        }



        return 0;
    }
}