#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../GLOBAL_SOURCE/global.h"

#define LINE_STOCK 11

//----------------------------------------------------

void printStockPreco(int codigoProduto, char* data) {

	//----------------------------------------------------
	//         MOSTRAR O STOCK
	//----------------------------------------------------
	
	//Abrir o ficheiro de Stocks
	int fd_stock = open(PATH_STOCK, O_RDONLY, 0666);

	//----------------------------------------------------

	//Caracteres lidos pelo readln
	int n = 0, codigoAtual = 1;
	//Ler para o buffer
	char buffer[MAX_LINE];

	do 
	{
		n = readln(fd_stock, buffer, MAX_LINE);

		codigoAtual++;

	} while(codigoAtual <= codigoProduto && n > 0);

	//Caso nao exista o codigo
	if (codigoAtual == codigoProduto) 
		return;

	int quantidadeStock = atoi(buffer);

	//Fechar o ficheiro de stocks
	close(fd_stock);

	//----------------------------------------------------
	//         MOSTRAR O PRECO
	//----------------------------------------------------

	//Abrir o ficheiro de artigos
	int fd_artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);
	//----------------------------------------------------

	int pos_leitura = LINE_ARTIGOS * (codigoProduto - 1);

	off_t offset = lseek(fd_artigo, pos_leitura, SEEK_SET);

	n = readln(fd_artigo, buffer, LINE_ARTIGOS);

	char **campos = tokenizeArtigo(campos, buffer);

	double precoLido = atof(campos[1]);

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%s Artigo: %d | Stock: %d | Preco: %lf\n", data, 
																	   codigoProduto, 
																	   quantidadeStock, 
																	   precoLido);
	
	int fd_pipe_escrita = open("../PipeVendas/pipePrintCliente", O_WRONLY);
	
	if(write(fd_pipe_escrita, 
		bufferEscrita, strlen(bufferEscrita))!=-1);

	close(fd_pipe_escrita);

	//Já nao é necessario o ficheiro artigos
	close(fd_artigo);
}

//----------------------------------------------------

void updateQuantidadeStock (int codigo, int novaQuantidade, char* data) {

	int fd_stock = open(PATH_STOCK, O_RDWR, 0666);

	int pos_leitura = (codigo-1) * LINE_STOCK;
	off_t offset = lseek(fd_stock, pos_leitura, SEEK_SET);

	char stockAntigo[MAX_LINE];
	int n = readln(fd_stock, stockAntigo, MAX_LINE);

	offset = lseek(fd_stock, pos_leitura, SEEK_SET);

	int finalQuantidade = atoi(stockAntigo) + novaQuantidade;
	sprintf(stockAntigo, "%010d\n", finalQuantidade);	

	if (write(fd_stock, stockAntigo, strlen(stockAntigo)) != -1);

	close(fd_stock);

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%s Quantidade de stock: %d\n", data, 
														      finalQuantidade);

	int fd_pipe_escrita = open("../PipeVendas/pipePrintCliente", O_WRONLY);
	
	if(write(fd_pipe_escrita, 
		bufferEscrita, strlen(bufferEscrita))!=-1);
	
	close(fd_pipe_escrita);
}

//----------------------------------------------------

void updateVenda (int codigo, int quantidade) 
{
	//Obter o preço de uma venda---------------------

	//Abrir o ficheiro de artigos
	int fd_artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);

	//Zerar o contador
	int codigoAtual = 0;
	int n = 0;
	char buffer[MAX_LINE];

	do 
	{
		n = readln(fd_artigo, buffer, MAX_LINE);		

		codigoAtual++;

	} while(codigoAtual < codigo && n > 0);

	//No buffer tenho a linha certa
	//Para guardar a divisao do buffer
	char **campos = tokenizeArtigo(campos, buffer);

	//Preco do artigo para calcular o montante
	double precoLido = atof(campos[1]);

	//Falta apenas acrescentar ao ficheiro de vendas o final
	int fd_vendas = open(PATH_VENDAS, O_APPEND | O_WRONLY, 0666);

	//Juntar a venda toda num buffer

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%d %d %.2lf\n", codigo, quantidade, quantidade*precoLido);	

	if(write(fd_vendas, bufferEscrita, strlen(bufferEscrita)) != -1);

	close(fd_vendas);
}

int main() 
{
	//Numero de chars lidos pelo read
	int n = 1;

	//Array de strings para guardar o comando lido
	char** campos;

	//Para guardar o comando lido
	char buffer[MAX_LINE];

	do 
	{
		char buffer[MAX_LINE];
		
		int fd;
		fd = open("../PipeVendas/pipeClienteVendas", O_RDONLY);

		while(1)
		{
			n = readln(fd, buffer, MAX_LINE);

			if(n <= 0)
				break;

			//Os comandos inseridos podem ser dois:
			//1: <codigo> -> mostra o stock
			//2: <codigo> <quantidade> -> atualiza o stock e mostra novo stock
			//Se tiver um espaço então é o 2º comando
			if(nr_spaces_in_string(buffer) == 2) 
			{				
				campos = tokenizePedidodServidor(buffer);

				int codigo = atoi(campos[1]), quantidade = atoi(campos[2]);

				if (quantidade > 0) 
					updateQuantidadeStock(codigo, quantidade, campos[0]);
				else	
				{	
					updateQuantidadeStock(codigo, quantidade, campos[0]);
					updateVenda(codigo, abs(quantidade));
				}
			} 
			else if (nr_spaces_in_string(buffer) == 1) {

				//Comando sem espaço		
				campos = tokenizeComandoCV(buffer);
				//Passo-lhe o codigo do produto em questao
				printStockPreco(atoi(campos[1]), campos[0]);
			}
		}
		
		close(fd);

	} while(n > 0 && buffer[0] != '\n');
	
}