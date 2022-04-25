#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int sendToServer(char* buffer){
	int client_server = open("cliente_servidor_fifo", O_WRONLY|O_TRUNC, 0666);
	if(client_server == -1){
		perror("cliente_servidor_fifo");
		return 1;
	}

	write(client_server, buffer, sizeof(buffer));

	return 0;
}


int main(int argc, char* argv[]){

	if(argc < 1){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}

	int bytesRead = 0;
	char buffer[1024];

	bytesRead = read(0, buffer, sizeof(buffer));
	if(sendToServer(buffer) != 0){
		perror("Error when trying to send to server");
		return 1;
	}

	int server_client = open("servidor_cliente_fifo", O_RDONLY, 0666);
	if(server_client == -1){
		perror("servidor_cliente_fifo");
		return 1;
	}

	while((bytesRead = read(server_client, buffer, sizeof(buffer))) > 0){
		write(1, buffer, bytesRead);
	}

	close(server_client);

	return 0;
}