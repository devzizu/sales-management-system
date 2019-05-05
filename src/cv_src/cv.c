//-------------------------------------------------------------------------------

/** @file cv.c
*
*	@brief Ficheiro que implementa o cliente do servidor de vendas.
*
*/

//-------------------------------------------------------------------------------

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

//-------------------------------------------------------------------------------

/**MACRO path base do pipe servidor >> cliente*/
#define BASE_PATH "../PipeVendas/pipePrintCliente"
/**MACRO path para o executavel do agregador*/
#define AGR_EXEC_PATH "../ag_src/ag"
/**MACRO nome do comando do agregador*/
#define AGREGADOR "ag"

//-------------------------------------------------------------------------------

/** Variaveis globais a este programa, erros retornados pelo servidor e reportados no terminal dos clientes */

/*Erro comando invalido*/
static char invalidCMD[44] = "[./cv] client inserted an invalid command.\n";
/*Erro codigo que nao existe*/
static char overflowCode[51] = "[./cv] client requested code that does not exist.\n";
/*Erro nao conseguiu fazer update ao stock*/
static char errorUpdateStock[51] = "[./cv] client requested stock update to negative.\n";

//-------------------------------------------------------------------------------

/** @brief Main: Processa um pedido de um cliente dado o seu PID explorando pipes individuais.
 *				 No fim remove o pipe do cliente.
 *  @param.
 *  @return 0 caso os pedidos tiverem sido processados corretamente.
*/

int main(int argc, char* argv[]) {

	//-------------------------------------------------

	//Process ID do cliente
	pid_t processID = getpid();
	//Identificador do cliente no servidor de vendas
	pid_t clientAnswerID;

	//-------------------------------------------------
	
	char pathCliente[200];
	sprintf(pathCliente, "%s%d", BASE_PATH, (int) processID);

	//FIFO para receber respostas do servidor
	mkfifo(pathCliente, 0600);
	
	int n, n_pipe_pedido;
	int fd_pipe_pedidos;

	int spaces = 0, quantidade = 0, codigo = 0, codigoPedido;
	int pipe_insertions = 0;

	char pedidoBuffer[MAX_LINE];
	char clientRequest[MAX_LINE];
	char serverAnswer[MAX_LINE];
	char resposta[MAX_LINE];

	int pipeEnvioServer = open("../PipeVendas/pipeClienteVendas", O_WRONLY);

	//Ficheiro que contém os comandos a serem lidos
	int pedidosClientes = open("../PipeVendas/VendasRepository.txt", O_RDONLY, 0666);

	int errorLog = open(PATH_ERRORLOG, O_WRONLY|O_APPEND, 0666);

	//int ret = fcntl(pipeEnvioServer, F_SETPIPE_SZ, 1024 * 1024);
	//if (ret < 0) printf("error size\n");

	int fd_rdwr = open(pathCliente, O_RDWR);

	int tipoDeResposta;
	int precoLido;
	int successScanf;
	
	n = 1;

	pid_t pid;

	while (n > 0) {

		//n = readln(pedidosClientes, clientRequest, MAX_LINE);	
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

				codigoPedido = is_number_int(clientRequest);

				if (codigoPedido > 0) {

					sprintf(pedidoBuffer, "%07d %010d %010d", processID, 
												      	 	  codigoPedido,
												      	 	  0);
				} else {

					if (write(errorLog, invalidCMD, strlen(invalidCMD)) != -1);

					continue;
				}

			} else if (spaces == 1) {

				successScanf = sscanf(clientRequest, "%d %d", &codigo, &quantidade);

				if (successScanf == 2) {

					sprintf(pedidoBuffer, "%07d %010d %010d", processID, 
						                             	 	  codigo,
						                              		  quantidade);
				} else {

					if (write(errorLog, invalidCMD, strlen(invalidCMD)) != -1);

					continue;					
				}

			} else {
				
				if (write(errorLog, invalidCMD, strlen(invalidCMD)) != -1);
				
				continue;
			}
			
			//Enviar pedido para o servidor
			if(write(pipeEnvioServer, pedidoBuffer, TAM_PEDIDO) != -1);

			if (read(fd_rdwr, serverAnswer, TAM_RESPOSTA) == -1);

			sscanf(serverAnswer, "%d %d %d %d %d", &tipoDeResposta,
													(int*) &clientAnswerID, 
													&codigo,
													&quantidade,
													&precoLido);
					
			switch (tipoDeResposta) {

				//Mostrar resultado de printStockPreco
				case 0: 
					sprintf(resposta, "Artigo: %d | Stock: %d | Preco: %d\n", codigo,
																			  quantidade,
																		      precoLido);
					if (write(1 , resposta, strlen(resposta))!=-1);
				
					break;

				case 1:
					sprintf(resposta, "Quantidade de stock: %d\n", quantidade);
					
					if (write(1 , resposta, strlen(resposta))!=-1);
				
					break;

				case 3:

					if (write(errorLog, overflowCode, strlen(overflowCode)) != -1);

					break;

				case 4:

					if (write(errorLog, errorUpdateStock, strlen(errorUpdateStock)) != -1);

					break;

				default: break;
		
			}
		}
	} 
	
	close(pipeEnvioServer);
	close(pedidosClientes);
	close(fd_rdwr);
	close(errorLog);

	if (remove(pathCliente) != -1);
}