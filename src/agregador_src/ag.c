#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <stdlib.h>

#include "global.h"

int main () {

	if (system("clear")!=-1);

	GHashTable *tableLines = NULL;

	tableLines = g_hash_table_new(g_str_hash, g_str_equal); 

 	int fd_vendas = open(PATH_VENDAS, O_RDONLY, 0666);

 	int n = 1;

 	char key[MAX_LINE];
 	char value[MAX_LINE];
 	char buffer[MAX_LINE];
 
 	gboolean existeYet;
	
	n = readln(fd_vendas, buffer, MAX_LINE);
 
 	do {

 		sscanf(buffer, "%s %s", key, value);

 		existeYet = g_hash_table_insert (tableLines, 
 							 			strdup(key), 
 							 			strdup(buffer));

 		n = readln(fd_vendas, buffer, MAX_LINE);
 	
 	} while (n > 0);

	GList *l, 
		  *keys = g_hash_table_get_keys(tableLines),
		  *values = g_hash_table_get_values(tableLines);

	for (l = keys; l != NULL; l = l -> next) {
		printf("key=%s\n", (char*) l -> data);
	}

	for (l = values; l != NULL; l = l -> next) {
		printf("val=%s", (char*) l -> data);
	}

	close(fd_vendas);

	return 0;
}