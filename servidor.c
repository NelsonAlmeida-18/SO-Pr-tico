#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char* argv[]){
	
	if(argc != 2){
		fprintf(stderr, "Uso: [PATH] [SDSTORE DIR]%s\n", );
		return 1;
	}

	int servidor_fifo = open("servidor_fifo", O_WRONLY, 0666);
	int cliente_fifo = open("cliente_fifo", O_RDONLY, 0666);

	int ficheiro = open(argv[0], O_RDONLY, 0666);//verificar se é argv 0 ou 1
	
}