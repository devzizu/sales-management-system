//-------------------------------------------------------------------------------

/** @file compactador.c
*
*	@brief Ficheiro que contém a implementação do compactador do ficheiro strings.
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

/** @brief Função que calcula a percentagem de nomes obsuletos no ficheiro strings.
 *
 *  @return A percentagem de lixo.
*/

double lixo_strings () {

	int line_strings = linhasFicheiro(PATH_STRINGS);

	double total_trash = 0.0; //% of trash

	int fd_artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);

	int total_bytes = lseek(fd_artigo, 0, SEEK_END);

	int line_artigos = total_bytes / LINE_ARTIGOS;

	close(fd_artigo);

	total_trash = 1.0 - ((double) line_artigos / line_strings);

	return total_trash;
}

//*************************************************************************************

/** @brief Função que obtem a linha de um ficheiro num buffer.
 *		   Nota: função para um ficheiro de tamanho variavel (como por exemplo o strings.txt)
 *
 *  @param path ficheiro.
 *  @path line_to_search a linha.
 *  @return pointer para o buffer da linha.
*/

char* get_line_from_file (char *path, int line_to_search) {

	int fd_file = open(path, O_RDONLY, 0666);

	int n = 0, curr_line = 1, r = 0;
 	
 	//Linha lida
	char buffer[MAX_LINE];

	do {

 		n = readln(fd_file, buffer, MAX_LINE);
		
 		if (curr_line == line_to_search) {
 			r = 1;
 			break;
 		} 

 		curr_line++;

	} while (n > 0); 	

	buffer[strlen(buffer) - 1] = '\0';

	char *res;

	if (!r) 
		res = NULL;
	else { 
		res = strdup(buffer);
	}
	
	return res;
}

//*************************************************************************************

/** @brief Função que obtem o stock de uma linha do ficheiro stocks.
 *
 *  @param path ficheiro.
 *  @param path line_to_search a linha.
 *  @return pointer para o buffer da linha.
*/

int get_stock_from_file (char *path_stock, int referencia) {

	int fd_stock, pos_leitura, n;
	off_t offset;
	
	fd_stock = open(path_stock, O_RDONLY, 0666);
	pos_leitura = (referencia-1) * LINE_STOCK;
	offset = lseek(fd_stock, pos_leitura, SEEK_SET);

	char oldStock[MAX_LINE];

	n = read(fd_stock, oldStock, LINE_ARTIGOS);

	close(fd_stock);

	return atoi(oldStock);
}

int compactarStrings () {

	//-------------------------------------------------------------------

	int n = 0;
 	
	char buffer[MAX_LINE];

	int codigo_atual = 0, preco_atual, referencia_ant;

	//-------------------------------------------------------------------

	//Renomear os ficheiros para nomes temporários
	if (rename(PATH_ARTIGOS, PATH_TMP_ARTIGOS) != -1);
	if (rename(PATH_STOCK, PATH_TMP_STOCK) != -1);
	if (rename(PATH_STRINGS, PATH_TMP_STRINGS) != -1);

	int fd_novo_art = open(PATH_ARTIGOS, O_CREAT, 0666);
	int fd_novo_stk = open(PATH_STOCK, O_CREAT, 0666);
	int fd_novo_str = open(PATH_STRINGS, O_CREAT, 0666);

	close(fd_novo_art);
	close(fd_novo_str);
	close(fd_novo_stk);

	//-------------------------------------------------------------------

	int fd_artigos = open(PATH_TMP_ARTIGOS, O_RDONLY, 0666);

 	//Ler a primeira linha
	n = read(fd_artigos, buffer, LINE_ARTIGOS);

	codigo_atual++;

 	//Ler o resto das linhas
 	do {

 		sscanf(buffer, "%d %d", &referencia_ant, &preco_atual);

 		inserirArtigo(get_line_from_file(PATH_TMP_STRINGS, referencia_ant),
 					  preco_atual,
 					  get_stock_from_file(PATH_TMP_STOCK, codigo_atual));

 		n = read(fd_artigos, buffer, LINE_ARTIGOS);
 	
 		codigo_atual++;

	} while (n > 0);

	close(fd_artigos);

	//-------------------------------------------------------------------

	if (remove(PATH_TMP_ARTIGOS) != -1);
	if (remove(PATH_TMP_STOCK) != -1);
	if (remove(PATH_TMP_STRINGS) != -1);

	return 0;
}

//*************************************************************************************

/** @brief Main: efetua a compactação do ficheiro trazendo para a memória as partes essencias do ficheiro (não obsuletas).
 *
 *  @param path ficheiro.
 *  @return 0 se tudo correr bem.
*/

int main(int argc, char const *argv[]) {
	
	//-------------------------------------------------------------
	//Verificar se é necessário compactar

	printf("\n=> A correr o compactador...\n");

	double trash = 100 * lixo_strings();

	//Só executa o agregador se o ficheiro tiver mais de 20% de lixo

	printf("\n:: Strings obsuletos: %.2lf por cento. \n\n", trash);

	if (trash < 20.0) { 

		printf("Não será necessário correr o compactador [ menor do que 20 por cento ]\n\n");

		return 0;
	}

	//-------------------------------------------------------------

	if (compactarStrings() == 0)
		printf("\nCompactação feita...\n");

	//-------------------------------------------------------------

	return 0;
}