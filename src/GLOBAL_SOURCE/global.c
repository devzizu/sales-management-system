
//-------------------------------------------------------------------------------

/** @file global.c
*
*	@brief Ficheiro que contém funções auxiliares aos programas.
*
*/

//-------------------------------------------------------------------------------

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "global.h"

//-------------------------------------------------------------------------------

//Retorna a ultima linha onde foi inserido
int inserirArtigo (char* nome, int preco, int stock) {

	//Posicao onde vai ser inserido no ficheiro de strings
	int index = linhasFicheiro(PATH_STRINGS);

	//Abrir ambos os ficheiros
	//Não é preciso append pois faço lseek
	//Faço lseek para saber o valor que retorna, ou seja, o offset
	//...e guardo no index
	int fd_artigo = open (PATH_ARTIGOS, O_WRONLY, 0666);
	int fd_string = open (PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	char buffer[MAX_LINE];

	int linha_onde_inseriu = 1;

	if (fd_artigo != EOF && fd_string != EOF) {

		//Reposicionar o ficheiro de artigos no fim		
		linha_onde_inseriu = lseek(fd_artigo, 0, SEEK_END);

		//Guardar no buffer o novo artigo e a sua posição
		//o index representa o offset desde o inicio do ficheiro ate ao inicio da string do produto
		//A segunda parte é o preço do artigo
		sprintf(buffer, "%010d %010d\n", index, preco);
		if (write(fd_artigo, buffer, LINE_ARTIGOS) != -1);

		char nameBuffer[MAX_LINE];
		sprintf(nameBuffer, "%s\n", nome);
		//Acrescentar o novo nome ao fim do ficheiro strings
		if (write(fd_string, nameBuffer, strlen(nameBuffer)) != -1);
	}
	
	printf("Código do artigo: %d \n", 1 + (linha_onde_inseriu / LINE_ARTIGOS));

	//Fechar os fd's dos artigos
	close(fd_artigo);
	close(fd_string);

	//------------------------------------------------

	int fd_stock = open (PATH_STOCK, O_WRONLY|O_APPEND, 0666);

	char stockEmpty[MAX_LINE];

	sprintf(stockEmpty, "%010d\n", stock);

	if (write(fd_stock, stockEmpty, strlen(stockEmpty)) != -1);	

	close(fd_stock);

	//------------------------------------------------

	//Linha onde foi inserido
	return index;
}

//*************************************************************************************

int linhasFicheiro (char *path) {

	int n = 1, fd = open(path, O_RDONLY, 0666);
	int l = 0;
	char buffer[MAX_LINE];

	while (n > 0) {
		n = readline(fd, buffer, MAX_LINE);
		l++;
	}

	close(fd);

	return l;
}

//*************************************************************************************

int is_number_int (char* string) {

	int i;
	for (i = 0; string[i] != '\n' && string[i] != '\0'; i++) {

		if(!isdigit(string[i]))  
			return -1;
	}

	return atoi(string);
}

//************************************************************************************

int nr_spaces_in_string (char *string) {

	int i = 0, counter = 0;

	for (; i < strlen(string); i++) {
		if (string[i]==' ') 
			counter++;
	}

	return counter;
}

//*************************************************************************************

char** tokenizeComandoCV (char* string) {

	char **campos = (char**) malloc(sizeof(char*) * 2);

	campos[0] = strdup(strtok(string, " "));
	campos[1] = strdup(strtok(NULL, " "));

	return campos;
}

//*************************************************************************************

char** tokenizePedidodServidor (char *pedido) {

	char **campos = (char**) malloc(sizeof(char*) * 3);

	campos[0] = strdup(strtok(pedido, " "));
	campos[1] = strdup(strtok(NULL, " "));
	campos[2] = strdup(strtok(NULL, " "));

	return campos;
}

//*************************************************************************************

char** tokenizeComando (char* string) {

	char **campos = (char**) malloc(sizeof(char*) * 3);

	campos[0] = strdup(strtok(string, " "));
	campos[1] = strdup(strtok(NULL, " "));
	campos[2] = strdup(strtok(NULL, " "));

	return campos;
}

//*************************************************************************************

char** tokenizeArtigo (char **campos, char* string) {

	campos = (char**) malloc(sizeof(char*) * 2);

	campos[0] = strdup(strtok(string, " "));
	campos[1] = strdup(strtok(NULL, " "));

	return campos;
}

//*************************************************************************************

size_t readline (int fd, char* buffer, size_t nbyte) {

	size_t bytes_lidos = 0;
	int i  = 0;
	char c;

	do {

		i = 0;
		i = read(fd, &c, 1);

		if (i != 0) {
			buffer[bytes_lidos] = c;
			bytes_lidos++;
		}

	} while (i != 0 && bytes_lidos != nbyte && c != '\n');

	buffer[i]='\0';

	return bytes_lidos;
}

//*************************************************************************************

size_t readln (int fd, char *buf, size_t max) {

	int i, n;
    char ch = '\0';

    i = 0;
    do{
        n = read(fd, &ch, 1);
        buf[i]=ch;
        if(n == 1) i++;
    }while(i < max && ch !='\n' && n == 1);
    buf[i] = '\0';

    return i;
}

//*************************************************************************************

size_t readNbytesOnce (int fd, char* buf, size_t nr_bytes) {

	int n = 0;

	n = read(fd, buf, nr_bytes);

	buf[n+1] = '\0';

	return n;
}

//*************************************************************************************

int cat_file (char *file_to_cat) {

	char *cat[] = {"cat", file_to_cat, NULL};

	pid_t pid = fork();

	if (pid == 0) {

		execvp(cat[0], cat);

		_exit(0);

	} else if (pid > 0) {

		wait(NULL);
	}
}

