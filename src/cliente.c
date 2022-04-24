#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char* argv[]){

	if(argc < 1){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}

	int bytesRead = 0;
	char buffer[1024];
	int client_server = open("cliente_servidor_fifo", O_WRONLY, 0666);

	while((bytesRead = read(0, buffer, sizeof(buffer))) > 0){
		write(client_server, buffer, bytesRead);
	}

	close(client_server);

	int server_client = open("serviror_cliente_fifo", O_RDONLY, 0666);
	while((bytesRead = read(server_client, buffer, sizeof(buffer))) > 0){
		write(1, buffer, bytesRead);
	}

	close(server_client);

	return 0;
}