#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../GLOBAL_SOURCE/global.h"

int main(int argc, char* argv[]) {


	mkfifo("../PipeVendas/pipeClienteVendas", 0600);
	
	mkfifo("../PipeVendas/pipePrintCliente", 0600);

	int n, n_pipe_pedido;
	int fd, fd_pipe_pedidos;
	int spaces = 0;
	int quantidade = 0, codigo = 0, s;

	//Buffer que contem data + pedido para o sv
	char pedido[MAX_LINE];
	char pedidoBuffer[MAX_LINE];
	char clientRequest[MAX_LINE];
	char serverAnswer[MAX_LINE];
	char resposta[MAX_LINE];


	pid_t processID = getpid();
	pid_t clientAnswerID;

	fd = open("../PipeVendas/pipeClienteVendas", O_WRONLY);

	int pedidosClientes = open("../PipeVendas/VendasRepository.txt", O_RDONLY, 0666);

	do {

		//Enviar para o server------------------------------------------
		n = readln(pedidosClientes, clientRequest, MAX_LINE);

		if(clientRequest[0] != '\n') {

			spaces = nr_spaces_in_string(clientRequest);

			if (spaces == 0) {

				sprintf(pedidoBuffer, "%07d %08d %08d", processID, 
											      	 	atoi(clientRequest),
											      	 	0);

			} else if (spaces == 1) {

				sscanf(clientRequest, "%d %d", &codigo, &quantidade);

				sprintf(pedidoBuffer, "%07d %08d %08d", processID, 
					                             	 	codigo,
					                              		quantidade);
			}

			//Escrever o pedido para o pipe que vai até ao servidor
			if(write(fd, pedidoBuffer, TAM_PEDIDO) != -1);
		}

		//Ler resposta do server----------------------------------------

		fd_pipe_pedidos = open("../PipeVendas/pipePrintCliente", O_RDWR);

		n_pipe_pedido = read(fd_pipe_pedidos, serverAnswer, TAM_RESPOSTA);
		
		if (n_pipe_pedido == 0) break;

		int tipoDeResposta, codigoProduto, quantidadeStock;
		double precoLido;

		sscanf(serverAnswer, "%d %d %d %d %lf", &tipoDeResposta,
												(int*) &clientAnswerID, 
												&codigoProduto,
												&quantidadeStock,
												&precoLido);
		
		if (clientAnswerID == processID) { 

			switch (tipoDeResposta) {

				//Mostrar resultado de printStockPreco
				case 0: 
					sprintf(resposta, "Artigo: %d | Stock: %d | Faturado: %lf\n", codigoProduto,
																				quantidadeStock,
																				precoLido);
					break;

				case 1:
					sprintf(resposta, "Quantidade de stock: %d\n", quantidadeStock);
					break;

				default: break;
			}

			if (write(1 , resposta, strlen(resposta))!=-1);
			
			close(fd_pipe_pedidos);
		
		} else {
			
			if (write(n_pipe_pedido, serverAnswer, strlen(serverAnswer)) != -1);

			close(fd_pipe_pedidos);
		}

	} while (n > 0);

			close(fd);
}
