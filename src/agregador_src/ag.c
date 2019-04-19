
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <glib.h>
#include <gmodule.h>

#include "global.h"

int main () {

	if (system("clear") != -1);

	//Criar a arvore de vendasTable
	GHashTable *vendasTable = NULL;

	//inicializar a arvore de vendasTable
	vendasTable = g_hash_table_new(g_str_hash, g_str_equal); 

	//Abrir o ficheiro de vendasTable para leitura
 	int fd_vendasTable = open(PATH_VENDAS, O_RDONLY, 0666);

 	//Valor lido pelo readln
 	int n;

 	//Buffers para guardar os elementos lidos do ficheiro
 	char key[MAX_LINE];
 	char val[MAX_LINE];
 	char buffer[MAX_LINE];
 	
 	gboolean res;

 	//Ler a primeira linha
	n = readln(fd_vendasTable, buffer, MAX_LINE);
 	
 	//Ler o resto das linhas
 	do {

 		sscanf(buffer, "%s %s", key, val);

 		printf("buff=%s", buffer);

		GList *listaNova = NULL;

		if (g_hash_table_contains(vendasTable, key) == TRUE) {

			listaNova = g_hash_table_lookup(vendasTable, key);

		}
	
		listaNova = g_list_append(listaNova, 
								  strdup(buffer));

		//Inserir o elemento na hashtable
		res = g_hash_table_insert(vendasTable, strdup(key), listaNova);

		printf("Ja existe %s? %s\n\n", key, res==TRUE?"Nao":"Sim");


 		n = readln(fd_vendasTable, buffer, MAX_LINE);
 	
	} while (n > 0);

	close(fd_vendasTable);

	//--------------------------------------------------------------

	printf("\nElementos da hashtable (há %d keys):----------\n\n", g_hash_table_size(vendasTable));

	GList *auxKeys = NULL, 
		  *auxVal = NULL, 
		  *keys = NULL, 
		  *vals = NULL;
	
	keys = g_hash_table_get_keys(vendasTable);
	vals = g_hash_table_get_values(vendasTable);

	for (auxKeys = keys, auxVal = vals; 
		 auxKeys != NULL && auxVal != NULL; 
		 auxKeys = auxKeys -> next, auxVal = auxVal -> next) {

		printf("key = %s, values:\n", (char*) (auxKeys -> data));

		printf("\tval = %s\n", (char*) (auxVal -> data));

	}

	g_list_free(keys);
	g_list_free(vals);

	//--------------------------------------------------------------

	printf("\nElementos do ficheiro vendas:----------------\n");

	if (system("cat FILES/VENDAS.txt") != -1);

	return 0;
}