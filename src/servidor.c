#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


char* sdStoreDir="../SDStore-transf";

int execCommands(char **commands){

	int source = open(commands[0], O_RDONLY, 0666);
	if(source == -1){
		perror("Erro na origem\n");
		return 1;
	}

	int dest = open(commands[1], O_WRONLY|O_TRUNC|O_CREAT, 0666);
	if(dest == -1){
		perror("Erro no destino\n");
		return 1;
	}

	int status;
	int pipeline[1024][2];
	for(int i = 2; i < strlen(commands); i++){
		if(fork() == 0){
			if(pipe(pipeline[i-2]) == -1){
				perror("Erro na criação do pipe\n");
				return 1;
			}

			execlp(strcat("/.",commands[i]), strcat("/.", commands[i]), commands[1], commands[2], NULL);
		}
	}

	wait(NULL);
	close(source);
	close(dest);
	return 0;
}


char** receiveRequest(){
	int cliente_servidor = open("cliente_servidor_fifo",O_RDONLY, 0666);
	char buffer[1024];
	int bytesRead=read(cliente_servidor, buffer,sizeof(buffer));
	write(1,buffer,bytesRead);
	close(cliente_servidor);
	char **commands = malloc(sizeof(char)*50);

	if(strncmp(buffer, "proc-file", 8) == 0){
		char input[1024];
		strcpy(input, buffer);

		char* token;
		int i = 0;
		char* rest = input;
		while((token = strtok_r(rest, " ", &rest))){
			if(strncmp(token, "proc-file", 8) == 0 || strncmp(token, "./sdstore", 8) == 0){
				continue;
			}else{
				commands[i] = malloc(sizeof(char)*1024);
				commands[i] = strdup(token);
				i++;
			}
		}
	}

	return commands;
}

int sendStatus(char *status){
	int servidor_cliente=open("servidor_cliente_fifo",O_WRONLY,0666);
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
		char** commands =receiveRequest();
		execCommands(commands);
		sendStatus("done\n");
	}

	return 0;
}