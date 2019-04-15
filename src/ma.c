#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "string.h"
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

void atualizaNome (char *codigo, char *novoNome) {

	int fd_string = open(PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	//Acrescentar o novo nome no fim do ficheiro
	if (write(fd_string, novoNome, strlen(novoNome)) != -1);
	
	close(fd_string);

	//Atualizar o codigo do nome para a posicao nova do ficheiro
	int novoCodigo = linhasFicheiro(PATH_STRINGS) - 1;
	int antigoCodigo = atoi(codigo);

	int fd_artigo = open(PATH_ARTIGOS, O_RDWR, 0666);
	int n = 0, i = 0;
	char buffer[50];

	int offset = 0;

	lseek(fd_artigo, 0, SEEK_SET);	
	do {

		n = readln(fd_artigo, buffer, 50);
		buffer[n] = '\0';
		i++;
		offset+=n;

	} while (i < antigoCodigo);
	
	offset-=n;

	lseek(fd_artigo, offset, SEEK_SET);

	char **campos = (char**) malloc(sizeof(char*) * 2);

    campos = tokenizeArtigo(campos, buffer);

    char newString[strlen(buffer) + 10];

    sprintf(newString, "%d %.2lf", novoCodigo, atof(campos[1]));

    if (write(fd_artigo, newString, strlen(newString))!=-1)
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
			case 'n': atualizaNome(campos[1], campos[2]);
					  break;

			default: break;
		}


	}

	return 0;
}