#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[]){

	if(mkfifo("cliente_fifo", 0666) == -1){
		perror("cliente_fifo");
		return 1;
	}

	return 0;
}