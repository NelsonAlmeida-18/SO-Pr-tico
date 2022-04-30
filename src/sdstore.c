#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int makeRequest(int argc, char *argv[]){
	int cliente_servidor = open("cliente_servidor_fifo", O_WRONLY|O_TRUNC, 0666);
	char buffer[1024];
	argv++;
	int i = 1;
	while(i < argc){
		strcat(buffer, argv[i]);
	}

	printf("%s\n", buffer);

	//bytesRead=read(**argv, buffer,sizeof(buffer));
	write(cliente_servidor,buffer,sizeof(buffer));
	close(cliente_servidor);
	return 0;
}

int receiveInfo(){
	int servidor_cliente=open("servidor_cliente_fifo",O_RDONLY,0666);
	char buffer[1024];
	int bytesRead=read(servidor_cliente,buffer,sizeof(buffer));
	write(1,buffer,bytesRead);
	//close(servidor_cliente);
	return bytesRead;
}


int main(int argc, char* argv[]){

	if(argc < 1){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}


	while(1){
		if(makeRequest(argc, argv)==0){
			printf("Request made\n");
			while(receiveInfo()==0){
				printf("Waiting for server response\n");
			}
		}
	}

	return 0;
}