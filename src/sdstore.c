#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}


void makeRequest(int argc, char *argv[]){
	int cliente_servidor = open("cliente_servidor_fifo", O_WRONLY|O_TRUNC, 0666);
	if(cliente_servidor == -1){
		perror("Erro na abertura do fifo cliente_servidor (no cliente)\n");
		_exit(1);
	}
	char buffer[1024];
	argv++;
	int i = 1;

	
	int pid = getpid();
	char nomeDoFifo[1024] = "server_client_fifo_";
	char* pidCliente = itoa(pid, 10);
	strcat(nomeDoFifo, pidCliente);
	if((mkfifo(nomeDoFifo, 0666) == -1) && errno != EEXIST){
		perror("Erro na criação do fifo\n");
		_exit(1);
	}

	while(i < argc){ //-1 para ignorar o \n
		strcat(buffer, *argv);
		strcat(buffer, " ");
		i++;
		argv++;
	}

	strcat(buffer, " ");
	strcat(buffer, pidCliente);

	write(cliente_servidor, buffer, sizeof(buffer));
	close(cliente_servidor);

	char string[1024];
	int bytesRead = 0;

	int server_client_fifo = open(nomeDoFifo, O_RDONLY,0666);
	if(server_client_fifo == -1){
		perror("Erro na abertura do fifo server_client (no cliente)\n");
		_exit(1);
	}
	
	while((bytesRead = read(server_client_fifo, string, 1024)) > 0){
		write(1, string, bytesRead);
	}
	close(server_client_fifo);
	unlink(nomeDoFifo);

	_exit(0);
}



int main(int argc, char* argv[]){

	if(argc < 1){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}

	if(mkfifo("cliente_servidor_fifo", 0666) == -1 && errno != EEXIST){
		perror("cliente_servidor_fifo");
		return 1;
	}

	makeRequest(argc, argv);

	return 0;
}