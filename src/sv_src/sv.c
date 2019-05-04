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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../GLOBAL_SOURCE/global.h"

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

int updateQuantidadeStock (int codigo, int novaQuantidade, int clientID) {

	char pathCliente[200];
	sprintf(pathCliente, "%s%d", BASE_PATH, (int) clientID);

	int fd_stock = open(PATH_STOCK, O_RDWR, 0666);

	int pos_leitura = (codigo-1) * LINE_STOCK;
	off_t offset = lseek(fd_stock, pos_leitura, SEEK_SET);

	char stockAntigo[MAX_LINE];
	int n = read(fd_stock, stockAntigo, LINE_STOCK);

	int stockAnt = atoi(stockAntigo);

	int finalQuantidade = stockAnt + novaQuantidade;
	
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

	close(fd_stock);

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

void updateVenda (int codigo, int quantidade) {

	//Obter o preço de uma venda---------------------

	//Abrir o ficheiro de artigos
	int fd_artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);

	//Zerar o contador
	int codigoAtual = 0;
	int n = 0;
	char buffer[MAX_LINE];

	int pos_leitura = LINE_ARTIGOS * (codigo - 1);
	off_t offset = lseek(fd_artigo, pos_leitura, SEEK_SET);
	n = read(fd_artigo, buffer, LINE_ARTIGOS);

	//No buffer tenho a linha certa
	//Para guardar a divisao do buffer
	char **campos = tokenizeArtigo(campos, buffer);

	//Preco do artigo para calcular o montante
	int precoLido = atoi(campos[1]);

	//Falta apenas acrescentar ao ficheiro de vendas o final
	int fd_vendas = open(PATH_VENDAS, O_APPEND | O_WRONLY, 0666);

	//Juntar a venda toda num buffer

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%d %d %d\n", codigo, quantidade, quantidade*precoLido);	

	if(write(fd_vendas, bufferEscrita, strlen(bufferEscrita)) != -1);

	close(fd_vendas);
}

/** @brief Main: corre até receber um kill de processo.
 *
 *  @param.
 *  @return 0 quando o servidor é desligado.
*/

int main() {

	//Numero de chars lidos pelo read
	int n = 1, i = 0;

	//Array de strings para guardar o comando lido
	char** campos;

	//Para guardar o comando lido
	char buffer[MAX_LINE];
	
	int fd_pedidos = open("../PipeVendas/pipeClienteVendas", O_RDONLY, 0666);
	
	while(n > 0) {

		n = read(fd_pedidos, buffer, TAM_PEDIDO);

		if (n <= 0) break;

		campos = tokenizePedidodServidor(buffer);

		//Os comandos inseridos podem ser dois:
		//1: <codigo> -> mostra o stock
		//2: <codigo> <quantidade> -> atualiza o stock e mostra novo stock
		//Se tiver um espaço então é o 2º comando
		if(atoi(campos[2]) != 0) {

			int codigo = atoi(campos[1]), quantidade = atoi(campos[2]);

			if (quantidade > 0) 
				updateQuantidadeStock(codigo, quantidade, atoi(campos[0]));
			else	
			{	
				if (updateQuantidadeStock(codigo, 
										  quantidade, 
										  atoi(campos[0])) != -1) {

					updateVenda(codigo, abs(quantidade));
				}
			}
		} 
		else {

			//Passo-lhe o codigo do produto em questao
			printStockPreco(atoi(campos[1]), atoi(campos[0]));
		}
	}
	
	close(fd_pedidos);
	
	main();

	return 0;
}