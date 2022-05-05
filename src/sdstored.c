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
int maxNop,maxBCompress, maxBDecompress, maxGCompress, maxGDecompress,maxEncrypt, maxDecrypt;

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

	for(int j=0;j<i;j++){
		int pos = j+1;
		if(strncmp(config[j],"bcompress",9)==0){
			maxBCompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"bdecompress",11)==0){
			maxBDecompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"nop",3)==0){
			maxNop = atoi(config[pos]);
		}
		else if(strncmp(config[j],"gcompress",9)==0){
			maxGCompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"gdecompress",11)==0){
			maxGDecompress = atoi(config[pos]);
		}
		else if(strncmp(config[j],"encrypt",7)==0){
			maxEncrypt = atoi(config[pos]);
		}
		else if(strncmp(config[j],"decrypt",7)==0){
			maxDecrypt = atoi(config[pos]);
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
	int bcompress,bdecompress,nop,encrypt,decrypt,gcompress,gdecompress=0;
	char** config = openConfigFile(argv);
	char string[1024];
	queue = (char***)malloc(1024*sizeof(char**));

	char *funcDir = argv[2];

	while(1){
		int cliente_servidor = open("cliente_servidor_fifo",O_RDONLY, 0666);
		int servidor_cliente=open("servidor_cliente_fifo",O_WRONLY|O_TRUNC,0666);
		char buffer[1024];
		read(cliente_servidor, buffer,sizeof(buffer));
		close(cliente_servidor);

		//char **commands = malloc(sizeof(char)*1024);
		if(strncmp(buffer, "status",6)==0){
			if (queuesize>0){
				char message[2048]="";
				for(int i = 0; i<queuesize; i++){
					int j=0;
					char messageTemp[1024]="";
					char task[1280] = "";
					while(queue[i][j]){
						strcat(messageTemp,queue[i][j]); 
						strcat(messageTemp, " ");
						j++;
					}
					sprintf(task, "Task #%d: %s\n", i, messageTemp);
					strcat(message, task);
				}

				char transf1[40] = "";
				sprintf(transf1, "transf nop: %d/%d (running/max)\n", nop, maxNop);
				char transf2[40] = "";
				sprintf(transf2, "transf bcompress: %d/%d (running/max)\n", bcompress, maxBCompress);
				char transf3[40] = "";
				sprintf(transf3, "transf bdecompress: %d/%d (running/max)\n", bdecompress, maxBDecompress);
				char transf4[40] = "";
				sprintf(transf4, "transf gcompress: %d/%d (running/max)\n", gcompress, maxGCompress);
				char transf5[40] = "";
				sprintf(transf5, "transf gdecompress: %d/%d (running/max)\n", gcompress, maxGDecompress);
				char transf6[40] = "";
				sprintf(transf6, "transf encrypt: %d/%d (running/max)\n", encrypt, maxEncrypt);
				char transf7[40] = "";
				sprintf(transf7, "transf decrypt: %d/%d (running/max)\n", decrypt, maxDecrypt);

				strcat(message, transf1);
				strcat(message, transf2);
				strcat(message, transf3);
				strcat(message, transf4);
				strcat(message, transf5);
				strcat(message, transf6);
				strcat(message, transf7);
				write(servidor_cliente, message, strlen(message));
			}
			else{
				char *message="No processes in queue\n";
				write(servidor_cliente,message , strlen(message));
			}
			close(servidor_cliente);
		}
		if(strncmp(buffer, "proc-file", 9) == 0){
			char input[1024];
			strcpy(input, buffer);

			char* token;
			int i = 0;
			char* rest = input;
			int iters=0;
			queue[queuesize]=(char**)malloc(sizeof(char*)*1024);

			while((token = strtok_r(rest, " \n", &rest))){
				if(strncmp(token, "proc-file", 9) == 0 || strncmp(token, "./sdstore", 8) == 0){
					continue;
				}else{
					if(strncmp(token,"bcompress",9)==0 && bcompress<maxBCompress){
						bcompress+=1;
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					else if(strncmp(token,"bdecompress",11)==0 && bdecompress<maxBDecompress){
						bdecompress+=1;
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					else if(strncmp(token,"nop",3)==0 && nop<maxNop){
						nop+=1;
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					else if(strncmp(token,"gcompress",9)==0 && gcompress<maxGCompress){
						gcompress+=1;
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					else if(strncmp(token,"gdecompress",11)==0 && gdecompress<maxGDecompress){
						gdecompress+=1;
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					else if(strncmp(token,"encrypt",7)==0 && encrypt<maxEncrypt){
						encrypt+=1;
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					else if(strncmp(token,"decrypt",7)==0 && decrypt<maxDecrypt){
						decrypt+=1;
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					else if(iters==0 || iters==1){
						queue[queuesize][i]=(char*)malloc(sizeof(char)*1024);
						queue[queuesize][i]=strdup(token);
						i++;
						size++;
					}
					iters++;
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
				
				for(int i = lastCommands; i < queuesize; i++){

					int pos = 0;
					while(queue[i][pos] != NULL){
						if(strncmp(queue[i][pos], "nop", 3) == 0){
							nop--;
						}else if(strncmp(queue[i][pos], "bcompress", 9) == 0){
							bcompress--;
						}else if(strncmp(queue[i][pos], "bdecompress", 11) == 0){
							bdecompress--;
						}else if(strncmp(queue[i][pos], "gcompress", 9) == 0){
							gcompress--;
						}else if(strncmp(queue[i][pos], "gdecompress", 11) == 0){
							gdecompress--;
						}else if(strncmp(queue[i][pos], "encrypt", 7) == 0){
							encrypt--;
						}else if(strncmp(queue[i][pos], "decrypt", 7) == 0){
							decrypt--;
						}
						pos++;
					}

					int cont = 2;
					int j = 0;

					if(pipe(pipeline[j]) == -1){
							perror("Erro na criação do pipe\n");
							return 1;
					}

					if(cont == 2){
						if(fork() == 0){

							dup2(source, 0);
							close(source);

							close(pipeline[j][0]);
							dup2(pipeline[j][1], 1);
							close(pipeline[j][1]);

							char buffer[100];
							strcpy(buffer, funcDir);
							strcat(buffer, queue[i][cont]);

							execlp(buffer, buffer, NULL);
							perror("Erro na execução da primeira transformação\n");

							_exit(0);
						}
						close(pipeline[j][1]);
						cont++;
						j++;
					}

					while(cont < pos-1){
						if(pipe(pipeline[j]) == -1){
							perror("Erro na criação do pipe\n");
							return 1;
						}

						if(fork() == 0){
							dup2(pipeline[j-1][0], 0);
							close(pipeline[j-1][0]);

							close(pipeline[j][0]);
							dup2(pipeline[j][1], 1);
							close(pipeline[j][1]);

							char buffer[100];
							strcpy(buffer, funcDir);
							strcat(buffer, queue[i][cont]);

							execlp(buffer, buffer, NULL);
							perror("Erro na execução nas transformações intermédias\n");

							_exit(0);
						}
						close(pipeline[j][1]);
						close(pipeline[j-1][0]);
						cont++;
						j++;
					}

					if(cont == pos-1){
						if(fork() == 0){	
						    dup2(pipeline[j-1][0], 0);
							close(pipeline[j-1][0]);
							
							dup2(dest, 1);
							close(dest);

							char buffer[100];
							strcpy(buffer, funcDir);
							strcat(buffer, queue[i][cont]);

							execlp(buffer, buffer, NULL);
							perror("Erro na execução da funcionalidade\n");

							_exit(0);
						}
						close(pipeline[j-1][0]);
						cont++;
					}

				}
				lastCommands+=1;
				strcpy(string,"Concluded\n");
				write(servidor_cliente, string, strlen(string));
				close(servidor_cliente);

				wait(NULL);
			}
		}
	}
	return 0;
}
