#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char* argv[]){
	
	if(argc != 3){
		fprintf(stderr, "Uso: [PATH] [SDSTORE PATH]\n");
		return 1;
	}

	int ficheiro = open(argv[0], O_RDONLY, 0666);//verificar se é argv 0 ou 1
	if(ficheiro == -1){
		perror("Erro na abertura do config.txt");
		return 1;
	}
	
	int client_server = open("cliente_servidor_fifo", O_RDONLY, 0666);
	if(client_server == -1){
		perror("Erro na abertura do pipe");
		return 1;
	}

	int bytesRead = 0;
	char buffer[1024];

	while((bytesRead = read(client_server, buffer, sizeof(buffer))) > 0){
		write(1, buffer, bytesRead);
	}

	close(client_server);

	int server_client = open("servidor_cliente_fifo", O_WRONLY|O_TRUNC, 0666);
	if(server_client == -1){
		perror("servidor_cliente_fifo");
		return 1;
	}

	while((bytesRead = read(ficheiro, buffer, sizeof(buffer))) > 0){
		write(server_client, buffer, bytesRead);
	}

	close(server_client);
	close(ficheiro);

	return 0;

}