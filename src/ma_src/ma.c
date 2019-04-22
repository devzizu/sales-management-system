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

	//Novo offset do nome do produto alterado no ficheiro strings
	int newIndex = linhasFicheiro(PATH_STRINGS);

	int fd_string = open(PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	//Acrestar o novo nome ao fim, ja tem \n
	if (write(fd_string, novoNome, strlen(novoNome)) != -1);

	//Já não preciso mais desse ficheiro, logo fecho-o
	close(fd_string);

	//-----------------------------------------------------------

	//Renomear o ficheiro de artigos para um temporario
	if (rename(PATH_ARTIGOS, PATH_TMP_ARTIGOS) == 0);

	//Abrir o ficheiro tmp para leitura
	int fd_tmp = open(PATH_TMP_ARTIGOS, O_RDONLY, 0666);

	//Abrir novo ficheiro para guardar o ficheiro final com o nome que tinha antes
	int fd_novoArtigos = open(PATH_ARTIGOS, O_CREAT|O_TRUNC|O_WRONLY, 0666);

	//Contador para as linhas
	int linha_atual = 0;
	//Buffer para guardar o resultado das leituras
	char buffer[MAX_LINE];
	//n -> representa o numero de bytes lidos do readline
	int n = 0;
	while (linha_atual < ref_antiga) { 
	//Referencia antiga == Linha/Codigo que quero alterar do ficheiro artigos 

		n = readln(fd_tmp, buffer, MAX_LINE);
	
		if (linha_atual < (ref_antiga - 1)) {

			if (write(fd_novoArtigos, buffer, strlen(buffer)) != -1);
		
		}

		linha_atual++;
	}

	//-----------------------------------------------------------

	//Sacar o preco que estava na linha que quero alterar
	//Fazendo sscanf do buffer
	char precoAntigo[MAX_LINE], tmp[MAX_LINE];
	sscanf(buffer, "%s %s", tmp, precoAntigo);

	//Guardar tudo num buffer de escrita final
	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%d %s\n", newIndex
								    , precoAntigo);	

	//Escrever a linha nova no ficheiro novo de artigos
	//Esta é append as linhas que ja la estavam
    if(write(fd_novoArtigos, bufferEscrita, 
                strlen(bufferEscrita)) != -1);

	//-----------------------------------------------------------
    //Falta apenas colocar o resto das linhas do ficheiro tmp no novo

    //While: consegue ler algo do readln
    //Aqui o seek ja se encontra na posicao certa, ou seja,
    //logo a seguir da linha que se escolheu alterar dada pela ref_antiga
	while (n > 0) {

		n = readln(fd_tmp, buffer, MAX_LINE);

		if (write(fd_novoArtigos, buffer, strlen(buffer)) != -1);
	}

	//-----------------------------------------------------------

	//Remover o ficheiro temporario, já nao sera preciso
	if (remove(PATH_TMP_ARTIGOS) == 0);

	//Fechar os fd's dos ficheiros
	close(fd_tmp);
	close(fd_novoArtigos);

	//-----------------------------------------------------------

}

void atualizaPreco (int referencia, double novoPreco) {

	//Renomear o ficheiro de artigos para um temporario
	if (rename(PATH_ARTIGOS, PATH_TMP_ARTIGOS) == 0);

	//Abrir o ficheiro tmp para leitura
	int fd_tmp = open(PATH_TMP_ARTIGOS, O_RDONLY, 0666);

	//Abrir novo ficheiro para guardar o ficheiro final com o nome que tinha antes
	int fd_novoArtigos = open(PATH_ARTIGOS, O_CREAT|O_TRUNC|O_WRONLY, 0666);

	//Contador para as linhas
	int linha_atual = 0;
	//Buffer para guardar o resultado das leituras
	char buffer[MAX_LINE];
	//n -> representa o numero de bytes lidos do readline
	int n = 0;
	while (linha_atual < referencia) { 
	//Referencia antiga == Linha/Codigo que quero alterar do ficheiro artigos 

		n = readln(fd_tmp, buffer, MAX_LINE);
	
		if (linha_atual < (referencia - 1)) {

			if (write(fd_novoArtigos, buffer, strlen(buffer)) != -1);
		
		}

		linha_atual++;
	}

	//Sacar o preco que estava na linha que quero alterar
	//Fazendo sscanf do buffer
	char precoAntigo[MAX_LINE], tmp[MAX_LINE];
	sscanf(buffer, "%s %s", tmp, precoAntigo);

	//Guardar tudo num buffer de escrita final
	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%s %.2lf\n", tmp
								     , novoPreco);	

	//Escrever a linha nova no ficheiro novo de artigos
	//Esta é append as linhas que ja la estavam
    if(write(fd_novoArtigos, bufferEscrita, 
                strlen(bufferEscrita)) != -1);

	//-----------------------------------------------------------
    //Falta apenas colocar o resto das linhas do ficheiro tmp no novo

    //While: consegue ler algo do readln
    //Aqui o seek ja se encontra na posicao certa, ou seja,
    //logo a seguir da linha que se escolheu alterar dada pela ref_antiga
	while (n > 0) {

		n = readln(fd_tmp, buffer, MAX_LINE);

		if (write(fd_novoArtigos, buffer, strlen(buffer)) != -1);
	}

	//-----------------------------------------------------------

	//Remover o ficheiro temporario, já nao sera preciso
	if (remove(PATH_TMP_ARTIGOS) == 0);

	//Fechar os fd's dos ficheiros
	close(fd_tmp);
	close(fd_novoArtigos);

	//-----------------------------------------------------------

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