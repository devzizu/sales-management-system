#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
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

int main(int argc, char* argv[]) {

	pid_t processID = getpid();
	pid_t clientAnswerID;

	//FIFO de envio ao servidor
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

	//Ficheiro que contém os comandos a serem lidos
	int pedidosClientes = open("../PipeVendas/VendasRepository.txt", O_RDONLY, 0666);
	
	int pipeEnvioServer = open("../PipeVendas/pipeClienteVendas", O_WRONLY);

	int ret = fcntl(pipeEnvioServer, F_SETPIPE_SZ, 1024 * 1024);
	if (ret < 0) printf("error size\n");

	do {

		n = readln(pedidosClientes, clientRequest, MAX_LINE);	

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

		//Contar as inserções que este cliente fez
		//Depois uso este numero para verificar se o pipe esta vazio
		pipe_insertions++;

	} while (n > 0);
	
	close(pipeEnvioServer);
	close(pedidosClientes);

	//Para debug
	printf("Acabei de ler os comandos...\n");

	int tipoDeResposta;
	double precoLido;
	
	codigo = 1;

	//Abrir pipe de respostas do servidor
	int fd_read_pipe;
	int fd_write_pipe;
	
	fd_read_pipe = open(pathCliente, O_RDONLY);
	fd_write_pipe = open(pathCliente, O_WRONLY);
	
	//Enquanto tem coisas no pipe		
	while (pipe_insertions > 0) {

		if (read(fd_read_pipe, serverAnswer, TAM_RESPOSTA) == -1);

		sscanf(serverAnswer, "%d %d %d %d %lf", &tipoDeResposta,
												(int*) &clientAnswerID, 
												&codigo,
												&quantidade,
												&precoLido);
				
		if (clientAnswerID == processID) { 

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

		} else {
			
			if (write(fd_write_pipe, serverAnswer, TAM_RESPOSTA)==-1);
		}

		pipe_insertions--;
	}		

	close(fd_read_pipe);
	close(fd_write_pipe);

	if (remove("../PipeVendas/pipeClienteVendas") != -1);
	if (remove(pathCliente) != -1);
}