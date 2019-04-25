#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../GLOBAL_SOURCE/global.h"

#define DATE_MAX 15

int isequal(char *date1, char* date2) {

	int r = 0;

	int h1, m1, s1, h2, m2, s2;

	sscanf(date1, "H%d:M%d:S%d", &h1, &m1, &s1);
	sscanf(date2, "H%d:M%d:S%d", &h2, &m2, &s2);

	return (h1 == h2 && m1 == m2 && s1 == s2);
}

int main(int argc, char* argv[]) {


	mkfifo("../PipeVendas/pipeClienteVendas", 0600);
	
	mkfifo("../PipeVendas/pipePrintCliente", 0600);

	int n, n_pipe_pedido;

	int fd, fd_pipe_pedidos;

	//Buffer que contem data + pedido para o sv
	char bufferGlobal[MAX_LINE];
	char dateID[DATE_MAX];
	char bufferPedido[MAX_LINE];
	char bufferResposta[MAX_LINE];
	char resposta[MAX_LINE];
	char data[DATE_MAX];

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	sprintf(dateID, "H%d:M%d:S%d", tm.tm_hour, 
								   tm.tm_min, 
								   tm.tm_sec);
		fd = open("../PipeVendas/pipeClienteVendas", O_WRONLY);

	while (bufferPedido[0] != '\n') {

	
		n = read(0, bufferPedido, MAX_LINE); // Substituir por MAX_LINE
	
		sprintf(bufferGlobal, "%s %s", dateID, bufferPedido);

		if(bufferPedido[0] != '\n') {

			if(write(fd, bufferGlobal, strlen(bufferGlobal)) != -1);
		}

		fd_pipe_pedidos = open("../PipeVendas/pipePrintCliente", O_RDWR);

		n_pipe_pedido = readln(fd_pipe_pedidos, bufferResposta, MAX_LINE);
		
		sscanf(bufferResposta, "%s %s", data, resposta);

		if (isequal(data, dateID)) {

			printf("%s", bufferResposta);
			close(fd_pipe_pedidos);
		
		} else {
			
			if (write(n_pipe_pedido, bufferResposta, 
				strlen(bufferResposta)) != -1);

			close(fd_pipe_pedidos);
		}

	}

			close(fd);
}
