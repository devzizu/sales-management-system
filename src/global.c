#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"

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