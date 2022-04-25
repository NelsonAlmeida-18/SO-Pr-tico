#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){

	if(mkfifo("cliente_servidor_fifo", 0666) == -1){
		perror("cliente_servidor_fifo");
		return 1;
	}

	if(mkfifo("servidor_cliente_fifo", 0666) == -1){
		perror("servidor_cliente_fifo");
		return 1;
	}

	return 0;
}