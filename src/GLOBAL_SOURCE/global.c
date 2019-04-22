#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

//Retorna a ultima linha onde foi inserido
int inserirArtigo (char* nome, double preco) {

	//Posicao onde vai ser inserido no ficheiro de strings
	int index = linhasFicheiro(PATH_STRINGS);

	//Abrir ambos os ficheiros
	//Não é preciso append pois faço lseek
	//Faço lseek para saber o valor que retorna, ou seja, o offset
	//...e guardo no index
	int fd_artigo = open (PATH_ARTIGOS, O_WRONLY, 0666);
	int fd_string = open (PATH_STRINGS, O_APPEND|O_WRONLY, 0666);

	char buffer[MAX_LINE];

	if (fd_artigo != EOF && fd_string != EOF) {

		//Reposicionar o ficheiro de artigos no fim		
		lseek(fd_artigo, 0, SEEK_END);

		//Guardar no buffer o novo artigo e a sua posição
		//o index representa o offset desde o inicio do ficheiro ate ao inicio da string do produto
		//A segunda parte é o preço do artigo
		sprintf(buffer, "%d %.2lf\n", index, preco);
		if (write(fd_artigo, buffer, strlen(buffer)) != -1);

		char nameBuffer[MAX_LINE];
		sprintf(nameBuffer, "%s\n", nome);
		//Acrescentar o novo nome ao fim do ficheiro strings
		if (write(fd_string, nameBuffer, strlen(nameBuffer)) != -1);
	}

	//Fechar os fd's dos artigos
	close(fd_artigo);
	close(fd_string);

	//Linha onde foi inserido
	return index;
}

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

int anySpaceInString (char *string) {

	int i = 0;

	for (; i < strlen(string); i++) {
		if (string[i]==' ') return 1;
	}

	return 0;
}

char** tokenizeComandoCV (char* string) {

	char **campos = (char**) malloc(sizeof(char*) * 2);

	campos[0] = strdup(strtok(string, " "));
	campos[1] = strdup(strtok(NULL, " "));

	return campos;
}

char** tokenizeComando (char* string) {

	char **campos = (char**) malloc(sizeof(char*) * 3);

	campos[0] = strdup(strtok(string, " "));
	campos[1] = strdup(strtok(NULL, " "));
	campos[2] = strdup(strtok(NULL, " "));

	return campos;
}

char** tokenizeArtigo (char **campos, char* string) {

	campos[0] = strdup(strtok(string, " "));
	campos[1] = strdup(strtok(NULL, " "));

	return campos;
}

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