#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <errno.h>
#include <poll.h>

#include "../GLOBAL_SOURCE/global.h"

#define BASE_PATH "../PipeVendas/pipePrintCliente"
#define AGR_EXEC_PATH "../ag_src/ag"
#define AGREGADOR "ag"

int main(int argc, char* argv[]) {

	//-------------------------------------------------

	//Process ID do cliente
	pid_t processID = getpid();
	//Identificador do cliente no servidor de vendas
	pid_t clientAnswerID;

	//-------------------------------------------------

	//FIFO de envio dos pedidos do cliente ao servidor
	mkfifo("../PipeVendas/pipeClienteVendas", 0600);	
	
	char pathCliente[200];
	sprintf(pathCliente, "%s%d", BASE_PATH, (int) processID);

	//FIFO para receber respostas do servidor
	mkfifo(pathCliente, 0600);
	
	int n, n_pipe_pedido;
	int fd_pipe_pedidos;

	int spaces = 0, quantidade = 0, codigo = 0;
	int pipe_insertions = 0;

	char pedidoBuffer[MAX_LINE];
	char clientRequest[MAX_LINE];
	char serverAnswer[MAX_LINE];
	char resposta[MAX_LINE];

	int pipeEnvioServer = open("../PipeVendas/pipeClienteVendas", O_WRONLY);

	//Ficheiro que contém os comandos a serem lidos
	int pedidosClientes = open("../PipeVendas/VendasRepository.txt", O_RDONLY, 0666);

	//int ret = fcntl(pipeEnvioServer, F_SETPIPE_SZ, 1024 * 1024);
	//if (ret < 0) printf("error size\n");

	int fd_rdwr = open(pathCliente, O_RDWR);

	int tipoDeResposta;
	double precoLido;
	
	n = 1;

	pid_t pid;

	while (n > 0) {

		n = readln(0, clientRequest, MAX_LINE);	
		
		if (strlen(clientRequest) <= 0) break; 

		if(clientRequest[strlen(clientRequest)-1] == '\n')
			clientRequest[strlen(clientRequest)-1]='\0';

		if (!strcmp(clientRequest, AGREGADOR)) {

			pid = fork();

			if (pid == 0) {

				execl(AGR_EXEC_PATH, "./ag", NULL);

				_exit(0);

			} else if (pid > 0) {

				wait(NULL);

			}
			
		} else {

			spaces = nr_spaces_in_string(clientRequest);

			if (spaces == 0 && strlen(clientRequest) > 0) {

				sprintf(pedidoBuffer, "%07d %08d %08d", processID, 
											      	 	atoi(clientRequest),
											      	 	0);

			} else if (spaces == 1) {

				sscanf(clientRequest, "%d %d", &codigo, &quantidade);

				sprintf(pedidoBuffer, "%07d %08d %08d", processID, 
					                             	 	codigo,
					                              		quantidade);
			}

			//Enviar pedido para o servidor
			if(write(pipeEnvioServer, pedidoBuffer, TAM_PEDIDO) != -1);

			if (read(fd_rdwr, serverAnswer, TAM_RESPOSTA) == -1);

			sscanf(serverAnswer, "%d %d %d %d %lf", &tipoDeResposta,
													(int*) &clientAnswerID, 
													&codigo,
													&quantidade,
													&precoLido);
					
			switch (tipoDeResposta) {

				//Mostrar resultado de printStockPreco
				case 0: 
					sprintf(resposta, "Artigo: %d | Stock: %d | Preco: %lf\n", codigo,
																				quantidade,
																				precoLido);
					break;

				case 1:
					sprintf(resposta, "Quantidade de stock: %d\n", quantidade);
					break;

				default: break;
		
			}

			if (write(1 , resposta, strlen(resposta))!=-1);
		}
	} 
	
	close(pipeEnvioServer);
	close(pedidosClientes);
	close(fd_rdwr);

//	if (remove("../PipeVendas/pipeClienteVendas") != -1);
	if (remove(pathCliente) != -1);
}