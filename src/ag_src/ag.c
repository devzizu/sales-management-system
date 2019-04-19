
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <glib.h>
#include <gmodule.h>
#include <time.h>

#include "global.h"

GList* joinList (GList *arg) {

	GList *agregada = g_list_alloc();	
	agregada = NULL;

	//So vou agregar se tiver tamanho maior que 1
	if (g_list_length(arg) > 1) {

		GList *aux = NULL;

		int cod, quant; 
		double mont;
		
		char bufferEscrita[MAX_LINE];

		int finalQuant = 0; 
		double finalMont = 0;

		for (aux = arg; aux != NULL; aux = aux -> next) {

			sscanf(aux -> data, "%d %d %lf", &cod, &quant, &mont);

			finalQuant += quant;
			finalMont  += mont;

		}

		sprintf(bufferEscrita, "%d %d %.2lf\n", cod, finalQuant, finalMont);
	
		agregada = g_list_prepend(agregada, strdup(bufferEscrita));

	} else agregada = arg;


	return agregada;
}

GHashTable* agregarHashTable (GHashTable *vendasTable) {

	//Criar nova hashtable
	GHashTable* new = g_hash_table_new(g_str_hash, g_str_equal);

	//Lista de keys e values da hashtable
	GList *keys = NULL, *vals = NULL;

	keys = g_hash_table_get_keys(vendasTable);
	vals = g_hash_table_get_values(vendasTable);

	//Apontadores para percorrer as listas keys e values
	GList *lKey = NULL, *lVal = NULL;

	//So para garantir que res é sempre false, ou seja, nao ha repetidos
	gboolean res;

	//Percorrer as listas
 	for (lKey = keys, lVal = vals; 
 		 lKey != NULL && lVal != NULL; 
 		 lKey = lKey -> next, lVal = lVal -> next) {

 		GList *nova = NULL;

 		nova = joinList(lVal -> data);

		res = g_hash_table_insert(new, 
					             strdup(lKey -> data), 
					             nova);

	}

	return new;
}

void hashTable_to_stdout (GHashTable *arg) {

	//Lista de keys e values da hashtable
	GList *vals = NULL;

	vals = g_hash_table_get_values(arg);

	//Apontador para percorrer as listas de values
	GList *lVal = NULL, *l = NULL;

	for (lVal = vals; lVal != NULL; lVal = lVal -> next) {
		
		for (l = lVal -> data; l != NULL; l = l -> next) {

			if (write(1, l -> data, strlen(l -> data)) == -1);

		}
	}
}

//Retorna 0 se tudo correr bem
int hashTable_to_ficheiro (char *path, GHashTable *arg) {

	//Sucesso
	int r = 0;

	//Lista de keys e values da hashtable
	GList *vals = NULL;

	vals = g_hash_table_get_values(arg);

	//Apontador para percorrer as listas de values
	GList *lVal = NULL, *l = NULL;

	//Abrir o ficheiro path para escrita
	int fd_vendas = open(path, O_CREAT|O_TRUNC|O_WRONLY, 0666);

	for (lVal = vals; lVal != NULL; lVal = lVal -> next) {
		
		for (l = lVal -> data; l != NULL; l = l -> next) {

			if (write(fd_vendas, l -> data, strlen(l -> data)) == -1) 
				r = 1;

		}
	}

	close(fd_vendas);

	return r;
}

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

		//Inicializar a lista a NULL
		GList *listaNova = g_list_alloc();
		listaNova = NULL;

		//Caso a key já exista
		if (g_hash_table_contains(vendasTable, key) == TRUE) {

			listaNova = g_hash_table_lookup(vendasTable, key);
		}

		listaNova = g_list_prepend(listaNova, strdup(buffer));

		//Inserir o elemento na hashtable
		res = g_hash_table_insert(vendasTable, strdup(key), listaNova);

		printf("Ja existe %s? %s\n\n", key, res==TRUE?"Nao":"Sim");

 		n = readln(fd_vendasTable, buffer, MAX_LINE);
 	
	} while (n > 0);

	close(fd_vendasTable);

	//--------------------------------------------------------------

	//Agregar a hashtable
	GHashTable *agregada = NULL;
	agregada = agregarHashTable(vendasTable);

	//--------------------------------------------------------------

	//Mandar para o ficheiro e stdout
	hashTable_to_stdout(agregada);

	//Nome do ficheiro é a data e hora atual, vou buscar ao sistema

	char datePath[MAX_LINE];

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	sprintf(datePath, "FILES/%d-%d-%dT%d:%d:%d", tm.tm_year + 1900, 
										   tm.tm_mon + 1, 
										   tm.tm_mday, 
										   tm.tm_hour, 
										   tm.tm_min, 
										   tm.tm_sec);

	printf("path=%s\n", datePath);

	hashTable_to_ficheiro(datePath, agregada);

	return 0;
}