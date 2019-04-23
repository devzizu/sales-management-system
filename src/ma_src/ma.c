#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../GLOBAL_SOURCE/global.h"

//-----------------------------------------------------------------

void atualizaNome (int ref_antiga, char *novoNome) {

	//-----------------------------------------------------------
	// Adicionar ao fim do ficheiro Strings o novo Nome

	//Novo offset do nome do produto alterado no ficheiro strings
	int newIndex = linhasFicheiro(PATH_STRINGS);

	int fd_string = open(PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	//Acrestar o novo nome ao fim, ja tem \n
	if (write(fd_string, novoNome, strlen(novoNome)) != -1);

	//Já não preciso mais desse ficheiro, logo fecho-o
	close(fd_string);

	//-----------------------------------------------------------

	int fd_artigo = open(PATH_ARTIGOS, O_RDWR, 0666);

	lseek(fd_artigo, 0, SEEK_SET);

	int pos_escrita = (ref_antiga-1) * LINE_ARTIGOS;
	
	off_t offset = lseek(fd_artigo, pos_escrita, SEEK_SET);

	char buffer[MAX_LINE];
	int n;
	n = readln(fd_artigo, buffer, MAX_LINE);

	char precoAntigo[MAX_LINE], tmp[MAX_LINE];
	sscanf(buffer, "%s %s", tmp, precoAntigo);

	sprintf(buffer, "%08d %s\n", newIndex
							   , precoAntigo);	

	offset = lseek(fd_artigo, -LINE_ARTIGOS, SEEK_CUR);

	if (write(fd_artigo, buffer, LINE_ARTIGOS) != -1);
	
	close(fd_artigo);

	//-----------------------------------------------------------

}

void atualizaPreco (int referencia, double novoPreco) {

	int fd_artigo = open(PATH_ARTIGOS, O_RDWR, 0666);

	int pos_escrita = (referencia-1) * LINE_ARTIGOS;

	off_t offset = lseek(fd_artigo, pos_escrita, SEEK_SET);

	char buffer[MAX_LINE];

	int n;
	n = readln(fd_artigo, buffer, MAX_LINE);

	printf("%s\n", buffer);

	//Sacar o preco que estava na linha que quero alterar
	//Fazendo sscanf do buffer
	char precoAntigo[MAX_LINE], tmp[MAX_LINE];
	sscanf(buffer, "%s %s", tmp, precoAntigo);

	//Guardar tudo num buffer de escrita final
	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%s %012.2lf\n", tmp
        								  , novoPreco);	

	offset = lseek(fd_artigo, -LINE_ARTIGOS, SEEK_CUR);

	//Escrever a linha nova no ficheiro novo de artigos
	//Esta é append as linhas que ja la estavam
    if(write(fd_artigo, bufferEscrita, LINE_ARTIGOS) != -1);

	close(fd_artigo);

}

int main () {

	//---------------------------------------------------------

	//Limpar o terminal
	if (system("clear") != -1);

	//---------------------------------------------------------

	//Return value do readline
	int n = 1;
	//Buffer onde fica armazenado o comando
	char buffer[MAX_LINE];
	//Array de Strings que guarda os campos do comando
	char **campos;

	//---------------------------------------------------------

	while (n > 0) {

		//Ler o comando do stdin
		n = readln(0, buffer, MAX_LINE);

		//Separar o comando lido num array de strings
		campos = tokenizeComando(buffer);

		//Função a executar mediante o comando
		switch(campos[0][0]) {

			//Inserir novo artigo:> i <nome> <preco>
			case 'i': 
					inserirArtigo(campos[1], atof(campos[2]));
					break;

			//Alterar o nome do artigo:> n <codigo> <novo nome>
			case 'n': 
					atualizaNome(atoi(campos[1]), campos[2]);
					break;

			//Alterar o preco do artigo:> p <codigo> <novo preco>
			case 'p':
					atualizaPreco(atoi(campos[1]), atof(campos[2]));
					break;

			default: break;
		}

	}

	return 0;
}