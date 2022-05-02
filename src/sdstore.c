#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int receiveInfo(){
	int servidor_cliente=open("servidor_cliente_fifo",O_RDONLY,0666);
	char buffer[1024];
	int bytesRead;
	while((bytesRead=read(servidor_cliente,buffer,1024)>0)){
		write(1,buffer,bytesRead);
	}
	if(strcmp(buffer,"Done")==0)
		_exit(0);
	close(servidor_cliente);
	return bytesRead;
}


int makeRequest(int argc, char *argv[]){
	int cliente_servidor = open("cliente_servidor_fifo", O_WRONLY|O_TRUNC, 0666);
	char buffer[1024];
	argv++;
	int i = 1;
	while(i < argc){ //-1 para ignorar o \n
		strcat(buffer, *argv);
		strcat(buffer, " ");
		i++;
		argv++;
	}
	write(cliente_servidor, buffer, sizeof(buffer));
	close(cliente_servidor);

	char string[1024];
	int bytesRead = 0;
	int server_client_fifo = open("servidor_cliente_fifo", O_RDONLY);
	while((bytesRead = read(server_client_fifo, string, 1024)) > 0)
		write(1, string, bytesRead);
	close(server_client_fifo);


	return 0;
}



int main(int argc, char* argv[]){

	if(argc < 1){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}

	if(mkfifo("servidor_cliente_fifo", 0666) == -1 && errno != EEXIST){
		perror("servidor_cliente_fifo");
		return 1;
	}

	makeRequest(argc, argv);

	return 0;
}