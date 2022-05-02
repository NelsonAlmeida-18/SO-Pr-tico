#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


char* sdStoreDir="../SDStore-transf";
int size = 0;
char ***queue;
int queuesize = 0;
int lastCommands = 0;

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
		perror("USAGE: ./sdstore ...\n");
		return 1;
	}

	if(mkfifo("cliente_servidor_fifo", 0666) == -1 && errno != EEXIST){
		perror("cliente_servidor_fifo");
		return 1;
	}

	char** config = openConfigFile(argv);
	char string[1024];
	queue = (char***)malloc(1024*sizeof(char**));

	while(1){
		int cliente_servidor = open("cliente_servidor_fifo",O_RDONLY, 0666);
		int servidor_cliente=open("servidor_cliente_fifo",O_WRONLY|O_TRUNC,0666);
		char buffer[1024];
		read(cliente_servidor, buffer,sizeof(buffer));
		close(cliente_servidor);

		//char **commands = malloc(sizeof(char)*1024);
		if(strncmp(buffer, "status",6)==0){
			if (queuesize>0){
				char message[1024]="";
				for(int i = lastCommands; i<queuesize; i++){
					int j=0;
					char messageTemp[1024]="";
					while(queue[i][j]){
						strcat(messageTemp,queue[i][j]);
						j++;
					}
					strcat(message,messageTemp);
				}
				write(servidor_cliente, message, strlen(message));
				close(servidor_cliente);
			}
			else{
				char *message="No processes in queue\n";
				write(servidor_cliente,message , strlen(message));
			}
		}
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
					queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
					queue[queuesize][i]=strdup(token);
					i++;
					size++;
				}
			}
			queuesize+=1;
			strcpy(string, "Pending\n");
			write(servidor_cliente,string, strlen(string));

			//ExecCommands

			if (queuesize>0){

				strcpy(string, "Processing\n");
				write(servidor_cliente,string, strlen(string));


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

						dup2(source, 0);
						close(source);
						
						dup2(dest, 1);
						close(dest);

						char buffer[100];
						strcpy(buffer, "../SDStore-transf/");
						strcat(buffer, queue[0][i]);

						execlp(buffer, buffer, NULL);
						perror("Erro na execução da funcionalidade\n");

						_exit(0);
					}
				}
				lastCommands+=1;
				strcpy(string,"Done\n");
				write(servidor_cliente, string, strlen(string));


				wait(NULL);
				close(source);
				close(dest);
				close(servidor_cliente);
			}
		}
	}
	return 0;
}