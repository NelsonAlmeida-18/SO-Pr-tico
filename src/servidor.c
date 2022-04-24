#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char* argv[]){
	
	if(argc != 2){
		fprintf(stderr, "Uso: [PATH] [SDSTORE PATH]%s\n", );
		return 1;
	}

	if(mkfifo("cliente_servidor_fifo", 0666) == -1){
		perror("cliente_servidor_fifo");
		return 1;
	}

	if(mkfifo("servidor_cliente_fifo", 0666) == -1){
		perror("serviror_cliente_fifo");
		return 1;
	}

	int ficheiro = open(argv[0], O_RDONLY, 0666);//verificar se é argv 0 ou 1
	
	int pipe = open("serviror_cliente_fifo", O_RDONLY, 0666);

}