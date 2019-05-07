//-------------------------------------------------------------------------------

/** @file sv.c
*
*	@brief Ficheiro que implementa o servidor de vendas.
*
*/

//-------------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../GLOBAL_SOURCE/global.h"
#include "../GLOBAL_SOURCE/cache.h"

//-------------------------------------------------------------------------------

/** @brief Apresenta o stock e preco enviando a resposta por um pipe para o cliente com id clienteID
 * 
 *  @param codigoProduto codigo do produto.
 *  @param clientID PID do cliente.
*/

void printStockPreco(int codigoProduto, int clientID) {
	
	//----------------------------------------------------

	//Guarda a posicao donde se le no ficheiro
	int pos_leitura;
	//debug purpose
	off_t offset;

	//Caracteres lidos pelo readln
	int n = 0, codigoAtual = 1;
	//Ler para o buffer
	char buffer[MAX_LINE];

	//----------------------------------------------------

	//Abrir ficheiro de stocks
	int fd_stock = open(PATH_STOCK, O_RDONLY, 0666);

	//Ler o stock dado o codigo de produto

	pos_leitura = LINE_STOCK * (codigoProduto - 1);
	offset = lseek(fd_stock, pos_leitura, SEEK_SET);
	n = read(fd_stock, buffer, LINE_STOCK);

	int quantidadeStock = atoi(buffer);

	//Fechar o ficheiro de stocks
	close(fd_stock);

	//----------------------------------------------------

	//Abrir o ficheiro de artigos
	int fd_artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);

	pos_leitura = LINE_ARTIGOS * (codigoProduto - 1);
	offset = lseek(fd_artigo, pos_leitura, SEEK_SET);
	n = read(fd_artigo, buffer, LINE_ARTIGOS);

	//----------------------------------------------------

	//Path que define o pipe
	char pathCliente[200];
	sprintf(pathCliente, "%s%d", BASE_PATH, (int) clientID);

	//----------------------------------------------------

	//Caso o codigo exceda o tamanho do ficheiro
	if (n == 0) {

		char error_overflow[MAX_LINE];

		int fd_pipe_escrita = open(pathCliente, O_WRONLY);
	
		sprintf(error_overflow, "3 %07d %010d %010d %010d", 0, 0, 0, 0);

		if(write(fd_pipe_escrita, error_overflow, TAM_RESPOSTA)!=-1);

		close(fd_pipe_escrita);

		return;
	}
	
	char **campos = tokenizeArtigo(campos, buffer);

	int precoLido = atoi(campos[1]);

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "0 %07d %010d %010d %010d", clientID, 
													   codigoProduto, 
													   quantidadeStock, 
													   precoLido);
	

	int fd_pipe_escrita = open(pathCliente, O_WRONLY);

	if(write(fd_pipe_escrita, 
		bufferEscrita, TAM_RESPOSTA)!=-1);

	close(fd_pipe_escrita);

	//Já nao é necessario o ficheiro artigos
	close(fd_artigo);
}

//*************************************************************************************

/** @brief Função que atualiza a quantidade de stock caso ela se mantenha negativa
 *
 *  @param codigo codigo do produto.
 *  @param novaQuantidade quantidade.
 *  @param clientID PID do cliente.
 *  @return -1 caso nao seja possivel realizar esse update de stock.
*/

int updateQuantidadeStock (int codigo, int novaQuantidade, int clientID, int fd_stock) {

	char pathCliente[200];
	sprintf(pathCliente, "%s%d", BASE_PATH, (int) clientID);

	int pos_leitura = (codigo-1) * LINE_STOCK;
	off_t offset = lseek(fd_stock, pos_leitura, SEEK_SET);

	char stockAntigo[MAX_LINE];
	int n = read(fd_stock, stockAntigo, LINE_STOCK);

	int stockAnt = atoi(stockAntigo);

	int finalQuantidade = stockAnt + novaQuantidade;
	
	if (n == 0) {

		char error_overflow[MAX_LINE];

		int fd_pipe_escrita = open(pathCliente, O_WRONLY);
	
		sprintf(error_overflow, "3 %07d %010d %010d %010d", 0, 0, 0, 0);

		if(write(fd_pipe_escrita, error_overflow, TAM_RESPOSTA)!=-1);

		close(fd_pipe_escrita);

		return -1;
	}

	if (finalQuantidade < 0) {

		char error_overflow[MAX_LINE];

		int fd_pipe_escrita = open(pathCliente, O_WRONLY);
	
		sprintf(error_overflow, "4 %07d %010d %010d %010d", 0, 0, 0, 0);

		if(write(fd_pipe_escrita, error_overflow, TAM_RESPOSTA)!=-1);

		close(fd_pipe_escrita);

		return -1; //Indicação da não possibilidade de venda
	}

	char newStock[12];
	sprintf(newStock, "%010d\n", finalQuantidade);	

	offset = lseek(fd_stock, pos_leitura, SEEK_SET);
	if (write(fd_stock, newStock, LINE_STOCK) != -1);

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "1 %07d %010d %010d %010d", clientID, 
													   0, 
													   finalQuantidade, 
													   0);

	int fd_pipe_escrita = open(pathCliente, O_WRONLY);
	
	if(write(fd_pipe_escrita, 
		bufferEscrita, TAM_RESPOSTA)!=-1);

	close(fd_pipe_escrita);

	return 0;
}

//*************************************************************************************

/** @brief Função que adiciona uma venda ao ficheiro vendas.
 *		   (Nota: Não necessita de enviar respostas ao servidor)
 *  @param codigo codigo do produto.
 *  @param quantidade quantidade.
 *  @return -1 caso nao seja possivel realizar esse update de stock.
*/

void updateVenda (int codigo, int quantidade, CACHE cache) {

	int pos = lookup_code_cache(cache, codigo);

	int n, pos_leitura, precoLido;
	char buffer[MAX_LINE];
	off_t offset;
	char **campos;

	if (pos == -1) {
	
		//Abrir o ficheiro de artigos
		int fd_artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);

		//Zerar o contador
		n = 0;

		pos_leitura = LINE_ARTIGOS * (codigo - 1);
		offset = lseek(fd_artigo, pos_leitura, SEEK_SET);
		n = read(fd_artigo, buffer, LINE_ARTIGOS);

		close(fd_artigo);

		//No buffer tenho a linha certa
		//Para guardar a divisao do buffer
		campos = tokenizeArtigo(campos, buffer);

		//Preco do artigo para calcular o montante
		precoLido = atoi(campos[1]);

		CELULA nova = init_celula (nova, codigo, precoLido);

		cache = add_cache(cache, nova, &pos);
	
	} else {
		
		printf("ta na cache\n");
		
		precoLido = getPreco(cache, pos);
	}

	//Falta apenas acrescentar ao ficheiro de vendas o final
	int fd_vendas = open(PATH_VENDAS, O_APPEND | O_WRONLY, 0666);

	//Juntar a venda toda num buffer

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%d %d %d\n", codigo, quantidade, quantidade*precoLido);	

	if(write(fd_vendas, bufferEscrita, strlen(bufferEscrita)) != -1);

	close(fd_vendas);
}

void handle_sigint (int sig) {

	//-----------------------------------------------------------------------------------------

	printf("\n[LOG] Server shutting down...\n");

	if (remove("../PipeVendas/pipeClienteVendas") != -1);

	//-----------------------------------------------------------------------------------------

	printf("\n[LOG] Removing any logged clients...\n");

	//-----------------------------------------------------------------------------------------

	int fd_clients_log = open ("../PipeVendas/clientes.log", O_RDONLY, 0666);

	int bytes_linha = 44; //com \n

	char clienteINFO[45];
	int processID;

	while (read(fd_clients_log, clienteINFO, 45) > 0) {

		char *token = strtok(clienteINFO, " ");		
		token = strtok(NULL, " ");
		token = strtok(NULL, " ");

		processID = atoi(token);

		kill(processID, SIGINT);

	}

	close(fd_clients_log);

	fd_clients_log = open ("../PipeVendas/clientes.log", O_TRUNC, 0666);

	close(fd_clients_log);
	
	//-----------------------------------------------------------------------------------------

	_exit(0);	

}

/** @brief Main: corre até receber um kill de processo.
 *
 *  @param.
 *  @return 0 quando o servidor é desligado.
*/

int main() {

	//---------------------------------------------------------------------------

	CACHE cache_server = init_cache(10);
	
	//---------------------------------------------------------------------------

	int fd_stock = open(PATH_STOCK, O_RDWR, 0666);

	//FIFO de envio dos pedidos do cliente ao servidor
	mkfifo("../PipeVendas/pipeClienteVendas", 0600);	

	signal(SIGINT, handle_sigint);

	//Numero de chars lidos pelo read
	int n = 1, i = 0;

	//Array de strings para guardar o comando lido
	char** campos;

	//Para guardar o comando lido
	char buffer[MAX_LINE];
	
	int fd_pedidos = open("../PipeVendas/pipeClienteVendas", O_RDONLY, 0666);
	
	int pos;

	while(n > 0) {
	
		lseek(fd_stock, 0, SEEK_SET);

		n = read(fd_pedidos, buffer, TAM_PEDIDO);

		if (n <= 0) break;

		campos = tokenizePedidodServidor(buffer);

		if (atoi(campos[0]) == 11111) {

			pos = lookup_code_cache(cache_server, atoi(campos[1]));

			if (pos != -1) {

				printf("ja encontrei na cache, updating\n");

				setPreco(cache_server, pos, atoi(campos[2]));

			}

		} else {

			//Os comandos inseridos podem ser dois:
			//1: <codigo> -> mostra o stock
			//2: <codigo> <quantidade> -> atualiza o stock e mostra novo stock
			//Se tiver um espaço então é o 2º comando
			if(atoi(campos[2]) != 0) {

				int codigo = atoi(campos[1]), quantidade = atoi(campos[2]);

				if (quantidade > 0) 
					updateQuantidadeStock(codigo, quantidade, atoi(campos[0]), fd_stock);
				else if (quantidade < 0) {
					//printf("quantidade = %d\n", quantidade);
					if (updateQuantidadeStock(codigo, 
											  quantidade, 
											  atoi(campos[0]), 
											  fd_stock) != -1) {

						updateVenda(codigo, abs(quantidade), cache_server);
					}
				}
			} 
			else {

				//Passo-lhe o codigo do produto em questao
				printStockPreco(atoi(campos[1]), atoi(campos[0]));
			}


		}

		//print_cache(cache_server);
	}
	
	close(fd_pedidos);
	
	close(fd_stock);
	
	main();

	return 0;
}