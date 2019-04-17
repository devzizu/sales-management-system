#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"


void inserePedidoClie(char* codigoArtigo, char* quantidade)
{
	int quant = atoi(quantidade);

	int stock = open(PATH_STOCK, O_CREAT | O_RDWR, 0666);

	int n, r, linha_atual;
	r = linha_atual = 0;
	off_t offset = 0;

	char buffer[MAX_LINE];
	char **campos = (char**) malloc(sizeof(char*) * 2);

	// ABASTECIMENTO DE STOCK e DIMINUIÇÃO
	do 
	{
		n = readln(stock, buffer, MAX_LINE);

		offset += n;
		linha_atual++;

		campos = tokenizeArtigo(campos, buffer);

		if(!strcmp(campos[0], codigoArtigo))
			r = 1;

	} while(stock != EOF && !r);

	offset -= n;
	offset = lseek(stock, offset, SEEK_SET);

	int quantidade_atual = atoi(campos[1]);
	int newQuant = quantidade_atual + quant;
	
	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%s %d", codigoArtigo, newQuant);

	if(write(stock, bufferEscrita, strlen(bufferEscrita)) != -1);

	double preco = 0.0;

	if(quant < 0)
	{
		r = 0;
		int vendas = open(PATH_VENDAS, O_CREAT | O_APPEND | O_WRONLY, 0666);

		// PROCURAR PRECO DO ARTIGO
		int artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);

		do 
		{
			n = readln(artigo, buffer, MAX_LINE);
			campos = tokenizeArtigo(campos, buffer);

			if(!strcmp(campos[0], codigoArtigo))
			{	
				r = 1;
				preco = atof(campos[1]);
				if (write(1, campos[1], strlen(campos[1]))!=-1);
			}

		} while(artigo != EOF && !r);
 
		close(artigo);
		free(campos);
		quant = abs(quant);
		double faturado = preco * quant;
		sprintf(bufferEscrita, "%s %d %f\n", codigoArtigo, quant, faturado);

		if(write(vendas, bufferEscrita, strlen(bufferEscrita))!=-1);
		
		close(vendas);
	}

	close(stock);
}


void mostraStock(char* codigoArtigo)
{
	int r = 0, quant = 0;
	double preco = 0;

	char **campos = (char**) malloc(sizeof(char*) * 2);
	char buffer[MAX_LINE];

	int cod = atoi(codigoArtigo);
	int c;
	int artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);
	
	do 
	{
		readln(artigo, buffer, MAX_LINE);

		campos = tokenizeArtigo(campos, buffer);

		c = atoi(campos[0]);
		if(c == cod)
		{	
			r = 1;
			preco = atof(campos[1]);
		}

	} while(artigo != EOF && !r);

	close(artigo);

	int stock = open(PATH_STOCK, O_RDONLY, 0666);
	r = 0;
	
	do 
	{
		readln(stock, buffer, MAX_LINE);

		campos = tokenizeArtigo(campos, buffer);
		c = atoi(campos[0]);

		if(c == cod)
		{	
			r = 1;
			quant = atoi(campos[1]);
		}

	} while(stock != EOF && !r); 
	
	close(stock);

	char bufferEscrita[MAX_LINE];

	sprintf(bufferEscrita, "Código: %d | Quantidade no stock: %d | Preço/unid: %f\n", cod, quant, preco);
	
	if(write(1, bufferEscrita, strlen(bufferEscrita))!=-1); 
	
	free(campos);
}


int main()
{
	// TEM DE SER CRIADO NOUTRO FICHEIRO
	//mkfifo("pipeVendas", 0600);

	//int fd = open("pipeServidorVendas", O_RDONLY);
	int n = 1, r = 0;

	char buffer[MAX_LINE];

	while(n > 0)
	{
		n = read(0, buffer, MAX_LINE);

		//Separar o comando lido num array de strings
		for(int i = 0; i < n; i++)
		{
			if(buffer[i] == ' ')
				r = 1;
		}		

		if(!r)
		{
			//ŃÃO É UMA VENDA		
			mostraStock(buffer);
		}
		else
		{
			//É UMA VENDA
			char **campos = (char**) malloc(sizeof(char*) * 2);
			campos = tokenizeArtigo(campos, buffer);
			
			inserePedidoClie(campos[0], campos[1]);
		}
	}
}