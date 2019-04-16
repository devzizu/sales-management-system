#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

void inserirArtigo (char* nome, double preco, int *lastLine) {

	int fd_artigo = open (PATH_ARTIGOS, O_APPEND|O_WRONLY, 0666);
	int fd_string = open (PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	char buffer[strlen(nome) + 1];

	int index = 0;

	if (fd_artigo != EOF && fd_string != EOF) {

		index = linhasFicheiro(PATH_ARTIGOS);

		sprintf(buffer, "%d %.2lf\n", index, preco);
		if (write(fd_artigo, buffer, strlen(buffer)) != -1);

		sprintf(buffer, "%s\n", nome);
		if (write(fd_string, buffer, strlen(buffer)) != -1);
	}

	*lastLine = index;

	close(fd_artigo);
	close(fd_string);
}

void atualizaNome (int codigoAntigo, char *novoNome, int *lastLine) {

	//-----------------------------------------------------------

	//Abrir o ficheiro de strings para inserir no fim o novo nome
	int fd_string = open(PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	//Acrescentar o novo nome no fim do ficheiro de strings
	if (write(fd_string, novoNome, strlen(novoNome)) != -1);
	
	//Já nao é preciso usar mais o ficheiro de strings
	close(fd_string);

	//-----------------------------------------------------------

	//O codigo novo sera a posicao onde foi inserido o novo nome
	//Neste caso foi inserido no fim do ficheiro
	int novoCodigo = linhasFicheiro(PATH_STRINGS) - 1;

	//-----------------------------------------------------------

	//Renomear o ficheiro de artigos para um temporario
	if (rename(PATH_ARTIGOS, PATH_TMP_ARTIGOS) == 0);

	int fd_artigo = open(PATH_TMP_ARTIGOS, O_RDONLY, 0666);

	//Abrir novo ficheiro para guardar o ficheiro final com o nome que tinha antes
	int fd_novoArtigos = open(PATH_ARTIGOS, O_CREAT|O_TRUNC|O_WRONLY, 0666);

	//-----------------------------------------------------------

	int linha_atual = 0;
	char buffer[MAX_LINE]; int n;
	off_t offset = lseek(fd_novoArtigos, 0, SEEK_SET);

	while (linha_atual < codigoAntigo) {

		n = readln(fd_artigo, buffer, MAX_LINE);

		offset += n;
		linha_atual++;
	
		if ((linha_atual-1) < (codigoAntigo - 1)) {

			if (write(fd_novoArtigos, buffer, strlen(buffer)) != -1);
		
		}

	}

	//-----------------------------------------------------------

	char precoAntigo[MAX_LINE], tmp[MAX_LINE];
	sscanf(buffer, "%s %s", tmp, precoAntigo);

	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%d %s\n", novoCodigo>(*lastLine)?novoCodigo:(*lastLine), precoAntigo);
		
	//Após isto tenho o buffer criado com a linha alterada

	//-----------------------------------------------------------
	
	//Escrever a linha nova no ficheiro novo de artigos
    if(write(fd_novoArtigos, bufferEscrita, 
                strlen(bufferEscrita)) != -1);

    //while (fd_artigo != EOF) {
	while (n > 0) {

		n = readln(fd_artigo, buffer, MAX_LINE);

		offset+=n;

		if (write(fd_novoArtigos, buffer, strlen(buffer)) != -1);
	}

	//-----------------------------------------------------------

	lseek(fd_artigo, 0, SEEK_SET);
	lseek(fd_novoArtigos, 0, SEEK_SET);

	if (remove(PATH_TMP_ARTIGOS) == 0);

	if (rename(PATH_TMP_ARTIGOS, PATH_ARTIGOS) == 0);

	*lastLine = novoCodigo;

	close(fd_novoArtigos);
	close(fd_artigo);
}

int main () {

	//Limpar o terminal
	if (system("clear") != -1);

	//Return value do readline
	int n = 1;
	char **campos;
	char buffer[MAX_LINE];

	int *lastLine = (int*) malloc(sizeof(int));
	*lastLine = 0;

	while (n > 0) {

		//Ler o comando
		n = readln(0, buffer, MAX_LINE);

		//Separar o comando lido num array de strings
		campos = tokenizeComando(buffer);

		switch(campos[0][0]) {

			//Inserir novo artigo: i <nome> <preco>
			case 'i': inserirArtigo(campos[1], atof(campos[2]), lastLine);
					  break;

			//Alterar o nome do artigo: n <codigo> <novo nome>
			case 'n': atualizaNome(atoi(campos[1]), campos[2], lastLine);
					  break;

			default: break;
		}

	}

	free(lastLine);

	return 0;
}