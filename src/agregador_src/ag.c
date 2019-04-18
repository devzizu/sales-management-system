#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>

#include "global.h"

int main () {

	GHashTable *file_lines = NULL;

	file_lines = g_hash_table_new(g_str_hash, g_str_equal); 

 	int fd_vendas = open(PATH_VENDAS, O_RDONLY, 0666);


 	int n = 0;
 	char buffer[MAX_LINE];
 	do {

 		n = readln(fd_vendas, buffer, MAX_LINE);

 		gpointer codigo, quantidade, montante;
 
 		sscanf(buffer, "%d %d %lf", &codigo, &quantidade, &montante);

 		g_hash_table_insert (file_lines, strdup(buffer), codigo);

 	} while (n > 0);

 	GList *l, *keys;

 	keys = g_hash_table_get_keys(file_lines);

 	for (l = keys; l != NULL; l = l -> next) {
 		printf("%s", (char*) l->data);
 	}

	close(fd_vendas);

	return 0;
}