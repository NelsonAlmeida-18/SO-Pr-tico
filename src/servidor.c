#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


char* sdStoreDir="../SDStore-transf";


int receiveRequest(){
	int cliente_servidor = open("cliente_servidor_fifo",O_RDONLY, 0666);
	char buffer[1024];
	int bytesRead=read(cliente_servidor, buffer,sizeof(buffer));
	write(1,buffer,bytesRead);
	close(cliente_servidor);
	return 0;
}

int sendStatus(){
	int servidor_cliente=open("servidor_cliente_fifo",O_WRONLY,0666);
	char *status = "teste";
	write(servidor_cliente,status,sizeof(status));
	close(servidor_cliente);
	return 0;
}


int main(int argc, char* argv[]){

	if(argc !=3){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}

	while(1){
		receiveRequest();
		sendStatus();
	}

	return 0;
}