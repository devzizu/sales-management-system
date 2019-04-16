#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

void inserirArtigo (char* nome, double preco) {

	int fd_artigo = open (PATH_ARTIGOS, O_APPEND|O_WRONLY, 0666);
	int fd_string = open (PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	char buffer[strlen(nome) + 1];

	if (fd_artigo != EOF && fd_string != EOF) {

		int index = linhasFicheiro(PATH_ARTIGOS);

		sprintf(buffer, "%d %.2lf\n", index, preco);
		if (write(fd_artigo, buffer, strlen(buffer)) != -1);

		sprintf(buffer, "%s\n", nome);
		if (write(fd_string, buffer, strlen(buffer)) != -1);
	}

	close(fd_artigo);
	close(fd_string);
}

void atualizaNome (int codigoAntigo, char *novoNome) {

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

	int fd_artigo = open(PATH_ARTIGOS, O_RDWR, 0666);

	int linha_atual = 0;

	//Colocar o offset de escrita no inicio do ficheiro
	off_t offset = lseek(fd_artigo, 0, SEEK_SET);

	//Buffer para o readline com size MAX_LINE
	char buffer[MAX_LINE]; int n;
	do {

		n = readln(fd_artigo, buffer, MAX_LINE);

		offset+=n;
		linha_atual++;

	} while (linha_atual < codigoAntigo);

	offset-=n;

	offset = lseek(fd_artigo, offset, SEEK_SET);

	char precoAntigo[MAX_LINE], tmp[MAX_LINE];
	sscanf(buffer, "%s %s", tmp, precoAntigo);

	//Buffer que contem o novo codigo e preco de escrita em ARTIGOS.txt 
	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%d %s\0", novoCodigo, precoAntigo);

	if (write(fd_artigo, bufferEscrita, strlen(bufferEscrita)) != -1);

	close(fd_artigo);
}

int main () {

	//Limpar o terminal
	if (system("clear") != -1);

	//Return value do readline
	int n = 1;
	//Buffer lido pelo readline
	char buffer[MAX_LINE];
	char **campos;

	//Ler os comandos inseridos
	while (n > 0) {

		//Ler o comando
		n = readline(1, buffer, MAX_LINE);

		//Separar o comando lido num array de strings
		campos = tokenizeComando(buffer);

		switch(buffer[0]) {

			//Inserir novo artigo: i <nome> <preco>
			case 'i': inserirArtigo(campos[1], atof(campos[2]));
					  break;

			//Alterar o nome do artigo: n <codigo> <novo nome>
			case 'n': atualizaNome(atoi(campos[1]), campos[2]);
					  break;

			default: break;
		}


	}

	return 0;
}