#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int size = 0;
char ***queue;
int queuesize = 0;
int lastCommands = 0;


typedef struct commands{
	int nop;
	int bcompress;
	int bdecompress;
	int gcompress;
	int gdecompress;
	int encrypt;
	int decrypt;
	int maxnop;
	int maxbcompress;
	int maxbdecompress;
	int maxgcompress;
	int maxgdecompress;
	int maxencrypt;
	int maxdecrypt;	
} Commands;

typedef struct request{
	char* pidCliente;
	char* comando;
	char* source;
	char* destination;
	int nop;
	int bcompress;
	int bdecompress;
	int gcompress;
	int gdecompress;
	int encrypt;
	int decrypt;
	int totalCommands;

} Request;

char** openConfigFile(char* argv[], Commands commands){
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

	for(int j=0;j<i;j++){
		int pos = j+1;
		if(strncmp(config[j],"bcompress",9)==0){
			commands.maxbcompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"bdecompress",11)==0){
			commands.maxbdecompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"nop",3)==0){
			commands.maxnop = atoi(config[pos]);
		}
		else if(strncmp(config[j],"gcompress",9)==0){
			commands.maxgcompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"gdecompress",11)==0){
			commands.maxgdecompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"encrypt",7)==0){
			commands.maxencrypt = atoi(config[pos]);
		}
		else if(strncmp(config[j],"decrypt",7)==0){
			commands.maxdecrypt = atoi(config[pos]);
		}
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
	//DEPOIS TORNAR FUNÇAO VOID
	Request pedidos[1024];
	Commands commands;
	commands.nop=0;
	commands.bcompress=0;
	commands.bdecompress=0;
	commands.gcompress=0;
	commands.gdecompress=0;
	commands.encrypt=0;
	commands.decrypt=0;
	commands.maxnop=0;
	commands.maxbcompress=0;
	commands.maxbdecompress=0;
	commands.maxgcompress=0;
	commands.maxgdecompress=0;
	commands.maxencrypt=0;
	commands.maxdecrypt=0;
	char** config = openConfigFile(argv,commands);
	char string[1024];
	int queuesize=0;
	int lastCommands=0;
	char *funcDir = argv[2];

	while(1){
		char *debug = "Here";
		int cliente_servidor = open("cliente_servidor_fifo",O_RDONLY, 0666);
		int servidor_cliente;
		char buffer[1024];
		read(cliente_servidor, buffer,sizeof(buffer));
		close(cliente_servidor);
		
		if(strncmp(buffer, "status",6)==0){
			char nameOfFifo[1024] = "server_client_fifo_";
			char *token;
			char* rest=buffer;
			char* pid = malloc(1024*sizeof(char));
			while((token = strtok_r(rest, " \n", &rest))){
				if (strcmp(token, "status")!=0){
					strcpy(pid,token);
				}
			}
			strcat(nameOfFifo, pid);

			while((mkfifo(nameOfFifo, 0666)==-1) && errno != EEXIST){
				unlink(nameOfFifo);
			}

			servidor_cliente = open(nameOfFifo,O_WRONLY|O_TRUNC, 0666);

			if (queuesize>0){
				printf("Here");
				char message[2048]="";
				for(int i = 0; i<queuesize; i++){
					char messageTemp[1024]="";
					char task[1280] = "";
					strcat(messageTemp,pedidos[i].comando); 
					strcat(messageTemp, " ");
					sprintf(task, "Task #%d: %s\n", i, messageTemp);
					strcat(message, task);
				}

				char transf1[40] = "";
				sprintf(transf1, "transf nop: %d/%d (running/max)\n", commands.nop, commands.maxnop);
				char transf2[40] = "";
				sprintf(transf2, "transf bcompress: %d/%d (running/max)\n", commands.bcompress, commands.maxbcompress);
				char transf3[40] = "";
				sprintf(transf3, "transf bdecompress: %d/%d (running/max)\n", commands.bdecompress, commands.maxbdecompress);
				char transf4[40] = "";
				sprintf(transf4, "transf gcompress: %d/%d (running/max)\n", commands.gcompress, commands.maxgcompress);
				char transf5[40] = "";
				sprintf(transf5, "transf gdecompress: %d/%d (running/max)\n", commands.gdecompress, commands.maxgdecompress);
				char transf6[40] = "";
				sprintf(transf6, "transf encrypt: %d/%d (running/max)\n", commands.encrypt, commands.maxencrypt);
				char transf7[40] = "";
				sprintf(transf7, "transf decrypt: %d/%d (running/max)\n", commands.decrypt, commands.maxdecrypt);

				strcat(message, transf1);
				strcat(message, transf2);
				strcat(message, transf3);
				strcat(message, transf4);
				strcat(message, transf5);
				strcat(message, transf6);
				strcat(message, transf7);
				write(servidor_cliente, message, strlen(message));
				close(servidor_cliente);
				unlink(nameOfFifo);
			}
			else{
				char *message="No processes in queue\n";
				write(servidor_cliente,message , strlen(message));
				close(servidor_cliente);
				unlink(nameOfFifo);
			}
		}
		if(strncmp(buffer, "proc-file", 9) == 0){
			if(fork()==0){
				char input[1024];
				strcpy(input, buffer);

				char* token;
				int i = 0;
				char* rest = input;
				Request requests;

				requests.comando = malloc(1024*sizeof(char));
				requests.source=malloc(1024*sizeof(char));
				requests.destination=malloc(1024*sizeof(char));
				requests.pidCliente=malloc(1024*sizeof(char));
				requests.nop=0;
				requests.bcompress=0;
				requests.bdecompress=0;
				requests.gcompress=0;
				requests.gdecompress=0;
				requests.encrypt=0;
				requests.decrypt=0;
				requests.totalCommands=0;
				int iter=0;
				while((token = strtok_r(rest, " \n", &rest))){
					if(strncmp(token, "proc-file", 9) == 0 || strncmp(token, "./sdstore", 8) == 0){
						continue;
					}else{
						if(strncmp(token,"bcompress",9)==0){
							requests.comando=strcat(requests.comando, token);
							requests.comando=strcat(requests.comando, " ");
							requests.bcompress+=1;
							requests.totalCommands+=1;
							i++;
							size++;
						}
						
						else if(strncmp(token,"bdecompress",11)==0){
							requests.comando=strcat(requests.comando, token);
							requests.comando=strcat(requests.comando, " ");
							requests.bdecompress+=1;
							requests.totalCommands+=1;
							i++;
							size++;
						}
						else if(strncmp(token,"nop",3)==0 ){
							requests.comando=strcat(requests.comando, token);
							requests.comando=strcat(requests.comando, " ");
							requests.nop+=1;
							requests.totalCommands+=1;
							i++;
							size++;
						}
						else if(strncmp(token,"gcompress",9)==0 ){
							requests.comando=strcat(requests.comando, token);
							requests.comando=strcat(requests.comando, " ");
							requests.gcompress+=1;
							requests.totalCommands+=1;
							i++;
							size++;
						}
						else if(strncmp(token,"gdecompress",11)==0 ){
							requests.comando=strcat(requests.comando, token);
							requests.comando=strcat(requests.comando, " ");
							requests.gdecompress+=1;
							requests.totalCommands+=1;
							i++;
							size++;
						}
						else if(strncmp(token,"encrypt",7)==0 ){
							requests.comando=strcat(requests.comando, token);
							requests.comando=strcat(requests.comando, " ");				
							requests.encrypt+=1;
							requests.totalCommands+=1;
							i++;
							size++;
						}
						else if(strncmp(token,"decrypt",7)==0){
							requests.comando=strcat(requests.comando, token);
							requests.comando=strcat(requests.comando, " ");
							requests.decrypt+=1;
							requests.totalCommands+=1;
							i++;
							size++;
						}

						else if(iter==0){
							requests.source=strcpy(requests.source,token);
						}
						
						else if (iter==1){
							requests.destination=strcat(requests.destination, token);
						}
						else if(iter>1){
							requests.pidCliente = strcat(requests.pidCliente,token);
						}
						iter+=1;
					}
				}
				pedidos[queuesize]=requests;
				queuesize+=1;

				char nameOfFifo[1024] = "server_client_fifo_";
				strcat(nameOfFifo,requests.pidCliente);

				while((mkfifo(nameOfFifo, 0666)==-1) && errno != EEXIST){
					unlink(nameOfFifo);
				}

				servidor_cliente = open(nameOfFifo,O_WRONLY|O_TRUNC, 0666);
				strcpy(string, "Pending\n");
				write(servidor_cliente,string, strlen(string));
				//close(servidor_cliente);
			}
				//ExecCommands
		
			if (queuesize>0){
				if (fork()==0){
					Request cabeca = pedidos[lastCommands];
					char nameOfFifo[1024] = "server_client_fifo_";
					strcat(nameOfFifo,cabeca.pidCliente);

					//servidor_cliente = open(nameOfFifo, O_WRONLY|O_TRUNC, 0666);
					strcpy(string, "Processing\n");
					write(servidor_cliente,string, strlen(string));

					int source = open(cabeca.source, O_RDONLY, 0666);
					if(source == -1){
						perror("Erro na origem\n");
						return 1;
					}

					int dest = open(cabeca.destination, O_WRONLY|O_TRUNC|O_CREAT, 0666);
					if(dest == -1){
						perror("Erro no destino\n");
						return 1;
					}
					//int status;
					int pipeline[1024][2];
						
					/*  REFAZER
					int pos = 0;
					while(queue[i][pos] != NULL){
						if(strncmp(cab, "nop", 3) == 0){
							commands.nop--;
						}else if(strncmp(queue[i][pos], "bcompress", 9) == 0){
							commands.bcompress--;
						}else if(strncmp(queue[i][pos], "bdecompress", 11) == 0){
							commands.bdecompress--;
						}else if(strncmp(queue[i][pos], "gcompress", 9) == 0){
							commands.gcompress--;
						}else if(strncmp(queue[i][pos], "gdecompress", 11) == 0){
							commands.gdecompress--;
						}else if(strncmp(queue[i][pos], "encrypt", 7) == 0){
							commands.encrypt--;
						}else if(strncmp(queue[i][pos], "decrypt", 7) == 0){
							commands.decrypt--;
						}
						pos++;
					}
					*/

					if(cabeca.totalCommands==1){
						if(fork() == 0){
							dup2(source, 0);
							close(source);

							dup2(dest, 1);
							close(dest);

							char buffer[100];
							strcpy(buffer, funcDir);
							strcat(buffer, cabeca.comando);

							execlp(buffer, buffer, NULL);
							perror("Erro na execução da primeira transformação\n");

							_exit(0);
						}
					}
					else{
						int cont=1;
						int i = 0;
						char* commandSplit = cabeca.comando;
						char* token;
						while((token=strtok_r(commandSplit, " ", &commandSplit))){
							if(pipe(pipeline[i]) == -1){
								perror("Erro na criação do pipe\n");
								return 1;
							}

							if(cont == 1){
								if(fork() == 0){
									dup2(source, 0);
									close(source);

									dup2(pipeline[i][1], 1);
									close(pipeline[i][1]);

									char buffer[100];
									strcpy(buffer,funcDir);
									strcat(buffer, token);

									execlp(buffer, buffer, NULL);
									perror("Erro na execução da transformação inicial\n");

									_exit(0);
								}
								close(pipeline[i][1]);
							}
							else if(cont<cabeca.totalCommands){
								if(fork() == 0){
									dup2(pipeline[i-1][0], 0);
									close(pipeline[i-1][0]);

									close(pipeline[i][0]);
									dup2(pipeline[i][1], 1);
									close(pipeline[i][1]);

									char buffer[100];
									strcpy(buffer, funcDir);
									strcat(buffer, token);

									execlp(buffer, buffer, NULL);
									perror("Erro na execução nas transformações intermédias\n");

									_exit(0);
								}
								close(pipeline[i-1][0]);
								close(pipeline[i][1]);
							}
							else{
								if(fork() == 0){	
								    dup2(pipeline[i-1][0], 0);
									close(pipeline[i-1][0]);
									
									dup2(dest, 1);
									close(dest);

									char buffer[100];
									strcpy(buffer, funcDir);
									strcat(buffer, token);
									
									execlp(buffer, buffer, NULL);
									perror("Erro na execução da funcionalidade\n");

									_exit(0);
								}
								close(pipeline[i-1][0]);
							}
							cont+=1;
							i++;
						}
		
						lastCommands+=1;
						strcpy(string,"Concluded\n");
						write(servidor_cliente, string, strlen(string));
						//wait(NULL);
					}
					close(servidor_cliente);
					unlink(nameOfFifo);
					_exit(0);
				}
			}
			close(servidor_cliente);
			
		}
	}

	return 0;
}
