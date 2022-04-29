#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


char* sdStoreDir="../SDStore-transf";
int queuesize = 0;
int size;
char ***queue;

int sendStatus(char *status){
	int servidor_cliente=open("servidor_cliente_fifo",O_WRONLY|O_TRUNC,0666);
	write(servidor_cliente,status,sizeof(status));
	close(servidor_cliente);
	return 0;
}



int execCommands(){
	//sendStatus("processing\n");
	printf("Path1  %s\n", queue[0][0]);
	printf("Path2 %s\n",queue[0][1]);
	int source = open(queue[0][0], O_RDONLY, 0666);
	if(source == -1){
		perror("Erro na origem\n");
		return 1;
	}

	int dest = open(queue[0][1], O_WRONLY|O_TRUNC|O_CREAT, 0666);
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

			//execlp(strcat(commands[1]), strcat("./", commands[i]), commands[1], commands[2], NULL);
			//execlp("../SDStore-transf/bcompress", "../SDStore-transf/bcompress", "<enunciado.pdf>", "enunciado2", NULL);

			_exit(0);
		}
	}

	wait(NULL);
	close(source);
	close(dest);
	printf("Concluded\n");
	free(queue[0]);
	queuesize-=1;
	printf("%s\n",queue[0][0]);
	//como altero a queue??? quero adiantar o pointer que estava em 1 para 0
	//sendStatus("concluded\n");
	return 0;
}



int receiveRequest(){
	int cliente_servidor = open("cliente_servidor_fifo",O_RDONLY, 0666);
	char buffer[1024];
	read(cliente_servidor, buffer,sizeof(buffer));
	close(cliente_servidor);
	//char **commands = malloc(sizeof(char)*1024);

	if(strncmp(buffer, "proc-file", 9) == 0){
		char input[1024];
		strcpy(input, buffer);
		char* token;
		int i = 0;
		char* rest = input;
		queue[queuesize]=(char**)malloc(sizeof(char*)*1024);
		while((token = strtok_r(rest, " \n", &rest))){
			if(strncmp(token, "proc-file", 9) == 0 || strncmp(token, "./sdstore", 8) == 0){
				continue;
			}else{
				//commands[i] = malloc(sizeof(char)*1024);
				//commands[i] = strdup(token);
				queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
				queue[queuesize][i]=strdup(token);
				i++;
				size++;
			}
		}
		queuesize+=1;
		sendStatus("pending\n");
		return 0;
	}

	return 1;
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
	queue = (char***)malloc(1024*sizeof(char**));

	if(argc !=3){
		fprintf(stderr, "USAGE: ./sdstore ...\n");
		return 1;
	}

	//char** config = openConfigFile(argv);
	while(1){
		if(receiveRequest()==0){
			execCommands();
		}
	}

	return 0;
}