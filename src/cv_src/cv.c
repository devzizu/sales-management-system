#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../GLOBAL_SOURCE/global.h"

int main(int argc, char* argv[])
{
	int fd;
	mkfifo("../PipeVendas/pipeClienteVendas", 0600);
	
	char buffer[150]; // Substituir por MAX_LINE
	fd = open("../PipeVendas/pipeClienteVendas", O_WRONLY);

	//----------------------------------------------------

	int file = open("../PipeVendas/VendasRepository.txt", 0666);
	int n = 1;

	while(n > 0)
	{
		n = readln(file, buffer, 150);
		
		if(write(fd, buffer, n) != -1);
	}

	close(file);

	close(fd);
}