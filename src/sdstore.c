#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int makeRequest(int argc, char *argv[]){

	int cliente_servidor = open("cliente_servidor_fifo", O_WRONLY, 0666);
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
	return 0;
}

int receiveInfo(){
	int servidor_cliente=open("servidor_cliente_fifo",O_RDONLY,0666);
	char buffer[1024];
	int bytesRead;
	while((bytesRead=read(servidor_cliente,buffer,sizeof(buffer))>0)){
		write(1,buffer,sizeof(buffer));
	}
	if(strcmp(buffer,"Done")==0)
		_exit(0);
	close(servidor_cliente);
	return bytesRead;
}


int main(int argc, char* argv[]){

	if(argc < 1){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}


	while(1){
		makeRequest(argc, argv);
		receiveInfo();
	}

	return 0;
}