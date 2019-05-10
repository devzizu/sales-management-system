#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../GLOBAL_SOURCE/global.h"
#include "../GLOBAL_SOURCE/cache.h"

/**MACRO path base do pipe servidor >> cliente*/
#define BASE_PATH "../PipeVendas/pipePrintCliente"
/**MACRO path para o executavel do agregador*/
#define AGR_EXEC_PATH "../ag_src/ag"
/**MACRO nome do comando do agregador*/
#define AGREGADOR "a"

static char err_reading_num[24] = "[./ma] invalid number.\n";
static char err_reading_cmd[25] = "[./ma] invalid command.\n";

//-----------------------------------------------------------------

void atualizaNome (int ref_antiga, char *novoNome) {

	//-----------------------------------------------------------
	// Adicionar ao fim do ficheiro Strings o novo Nome

	//Novo offset do nome do produto alterado no ficheiro strings
	int newIndex = linhasFicheiro(PATH_STRINGS);

	int fd_string = open(PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	char nameBuffer[MAX_LINE];
	sprintf(nameBuffer, "%s\n", novoNome);
	
	//Acrestar o novo nome ao fim, ja tem \n
	if (write(fd_string, nameBuffer, strlen(nameBuffer)) != -1);

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

	sprintf(buffer, "%010d %s\n", newIndex
							    , precoAntigo);	

	offset = lseek(fd_artigo, -LINE_ARTIGOS, SEEK_CUR);

	if (write(fd_artigo, buffer, LINE_ARTIGOS) != -1);
	
	close(fd_artigo);

	//-----------------------------------------------------------

}

void atualizaPreco (int referencia, int novoPreco) {

	int fd_artigo = open(PATH_ARTIGOS, O_RDWR, 0666);

	int pos_escrita = (referencia-1) * LINE_ARTIGOS;

	off_t offset = lseek(fd_artigo, pos_escrita, SEEK_SET);

	char buffer[MAX_LINE];

	int n;
	n = read(fd_artigo, buffer, LINE_ARTIGOS);

	//Sacar o preco que estava na linha que quero alterar
	//Fazendo sscanf do buffer
	char precoAntigo[MAX_LINE], tmp[MAX_LINE];
	sscanf(buffer, "%s %s", tmp, precoAntigo);

	//Guardar tudo num buffer de escrita final
	char bufferEscrita[MAX_LINE];
	sprintf(bufferEscrita, "%s %010d\n", tmp
        							   , novoPreco);	

	//------------------------------------------------------------------------

	offset = lseek(fd_artigo, -LINE_ARTIGOS, SEEK_CUR);

	//Escrever a linha nova no ficheiro novo de artigos
	//Esta é append as linhas que ja la estavam
    if(write(fd_artigo, bufferEscrita, LINE_ARTIGOS) != -1);

	close(fd_artigo);

	//------------------------------------------------------------------------

	printf("updating preco\n");

	int pipeEnvioServer = open("../PipeVendas/pipeClienteVendas", O_WRONLY, 0666);

	if (pipeEnvioServer == -1) 
		return;

	char pedidoBuffer[MAX_LINE];
	int id_ma = 11111;

	sprintf(pedidoBuffer, "%07d %010d %010d", id_ma, 
	                             	 	      referencia,
	                            		      novoPreco);

	if(write(pipeEnvioServer, pedidoBuffer, TAM_PEDIDO) != -1);

	close(pipeEnvioServer);

}

int main () {

	//---------------------------------------------------------

	//Ficheiro onde se reportam erros de execução do programa
	int errorsLog = open (PATH_ERRORLOG, O_WRONLY|O_APPEND, 0666);

	//---------------------------------------------------------

	//Return value do readln
	int n = 1;

	//Buffer onde fica armazenado o comando introduzido pelo user
	char buffer[MAX_LINE];
	//Array de Strings que guarda os campos do comando
	char** campos;

	//Numeros lidos apos parse de erros
	int verified_int, verified_code;

	pid_t pid;

	//---------------------------------------------------------

	while (n > 0) {

		//Ler o comando do stdin
		n = readln(0, buffer, MAX_LINE);

		if(buffer[strlen(buffer)-1] == '\n')
			buffer[strlen(buffer)-1]='\0';

		if (!strcmp(buffer, AGREGADOR)) {

			int pipeEnvioServer = open("../PipeVendas/pipeClienteVendas", O_WRONLY, 0666);

			if (pipeEnvioServer == -1) {

				if(write(2, "Agregador temporariamente indisponivel [SERVER_SHUTDOWN]\n", 58)!=-1);

				continue;
			}

			char pedidoBuffer[MAX_LINE];
			int id_ag = 22222;

			sprintf(pedidoBuffer, "%07d %010d %010d", id_ag, 
			                             	 	      0,
			                            		      0);

			if(write(pipeEnvioServer, pedidoBuffer, TAM_PEDIDO) != -1);

			close(pipeEnvioServer);

		} else {

			if (nr_spaces_in_string(buffer) == 2) {

				//Separar o comando lido num array de strings
				campos = tokenizeComando(buffer);

				//Controlo da função a executar mediante o comando
				switch(campos[0][0]) {

					//Inserir novo artigo:> i <nome> <preco>
					case 'i': 

							//Caso corra bem esta função retorna > 0 (o número)
							verified_int = is_number_int(campos[2]);

							if (verified_int >= 0)
								inserirArtigo(campos[1], verified_int, 0);
							else
								if (write(errorsLog, err_reading_num, strlen(err_reading_num)) != -1);
							
							break;

					//Alterar o nome do artigo:> n <codigo> <novo nome>
					case 'n':
							verified_int = is_number_int(campos[1]);

							if (verified_int >= 0)
								atualizaNome(verified_int, campos[2]);
							else
								if (write(errorsLog, err_reading_num, strlen(err_reading_num)) != -1);

							break;

					//Alterar o preco do artigo:> p <codigo> <novo preco>
					case 'p':

							verified_int  = is_number_int(campos[2]);
							verified_code = is_number_int(campos[1]);

							if (verified_int >= 0 && verified_code >= 0)
								atualizaPreco(verified_code, verified_int);
							else
								if (write(errorsLog, err_reading_num, strlen(err_reading_num)) != -1);

							break;

					default: 

							if (write(errorsLog, err_reading_cmd, strlen(err_reading_cmd)) != -1);
						break;
				}
			} else {

				if (write(errorsLog, err_reading_cmd, strlen(err_reading_cmd)) != -1);
			}
		}
	}

	close(errorsLog);

	return 0;
}