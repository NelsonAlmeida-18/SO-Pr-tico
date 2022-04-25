#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


int makeRequest(){
	int cliente_servidor = open("cliente_servidor_fifo", O_WRONLY|O_TRUNC, 0666);
	char buffer[1024];
	int bytesRead=read(0, buffer,sizeof(buffer));
	write(cliente_servidor,buffer,bytesRead);
	close(cliente_servidor);
	return 0;
}

int receiveInfo(){
	int servidor_cliente=open("servidor_cliente_fifo",O_RDONLY,0666);
	char buffer[1024];
	int bytesRead=read(servidor_cliente,buffer,sizeof(buffer));
	write(1,buffer,bytesRead);
	close(servidor_cliente);
	return 0;
}


int main(int argc, char* argv[]){

	if(argc < 1){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}


	while(1){
		makeRequest();
		receiveInfo();
	}

	return 0;
}