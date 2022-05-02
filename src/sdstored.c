#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


char* sdStoreDir="../SDStore-transf";
int size = 0;

/*int sendStatus(char *status){
	int servidor_cliente=open("servidor_cliente_fifo",O_WRONLY|O_TRUNC,0666);
	printf("Status: %s\n",status);
	write(servidor_cliente,status,sizeof(status));
	close(servidor_cliente);
	return 0;
}*/

int execCommands(char **commands, char* argv[]){

	//sendStatus("Processing\n");
	printf("%s\n", commands[0]);

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

	//int status;
	int pipeline[1024][2];
	
	for(int i = 2; i < size; i++){
		if(fork() == 0){
			if(pipe(pipeline[i-2]) == -1){
				perror("Erro na criação do pipe\n");
				return 1;
			}

			dup2(source, 0);
			close(source);
			
			dup2(dest, 1);
			close(dest);

			char buffer[100];
			strcpy(buffer, argv[2]);
			strcat(buffer, commands[i]);
			printf("%s\n", buffer);

			execlp(buffer, buffer, NULL);
			perror("Erro na execução da funcionalidade\n");

			_exit(0);
		}
	}

	//sendStatus("Done\n");
	wait(NULL);
	close(source);
	close(dest);
	return 0;
}


char** receiveRequest(){
	int cliente_servidor = open("cliente_servidor_fifo",O_RDONLY, 0666);
	char buffer[1024];
	read(cliente_servidor, buffer,sizeof(buffer));
	close(cliente_servidor);
	char **commands = malloc(sizeof(char)*1024);

	int k = 0;
	char str[10];
	while(k < 10){
		str[k] = buffer[k];
		k++;
	}

	str[k] = '\0';
	printf("%s\n", str);
	printf("%d\n", strcmp(str, "proc-file"));

	if(strncmp(buffer, "proc-file", 9) == 0){
		char input[1024];
		strcpy(input, buffer);

		char* token;
		int i = 0;
		char* rest = input;
		while((token = strtok_r(rest, " \n", &rest))){
			if(strncmp(token, "proc-file", 9) == 0 || strncmp(token, "./sdstore", 8) == 0){
				continue;
			}else{
				commands[i] = malloc(sizeof(char)*1024);
				commands[i] = strdup(token);
				i++;
				size++;
			}
		}
	}

	return commands;
}


char** openConfigFile(char* argv[]){
	int configFile = open(argv[1], O_RDONLY, 0666);
	char buffer[1024];
	read(configFile, buffer, sizeof(buffer));

	char* token;
	int i = 0;
	char* rest = buffer;
	char** config = malloc(sizeof(char)*1024);

	while((token = strtok_r(rest, " \n", &rest))){
		config[i] = strdup(token);
		i++;
	}

	return config;
}


int main(int argc, char* argv[]){

	if(argc !=3){
		perror("USAGE: ./sdstored ...\n");
		return 1;
	}

	char** config = openConfigFile(argv);

	while(1){
		char** commands =receiveRequest();
		execCommands(commands, argv);
	}

	return 0;
}