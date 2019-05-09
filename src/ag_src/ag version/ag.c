//-------------------------------------------------------------------------------

/** @file ag.c
*
*	@brief Ficheiro que contém a implementação do agregador de vendas.
*
*/

//-------------------------------------------------------------------------------

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

#include "../GLOBAL_SOURCE/global.h"

//-------------------------------------------------------------------------------

/** @brief Função que agrega a lista de vendas com a mesma referencia.
 *         Acumula os montantes de todas as vendas e a quantidade vendida numa só venda.
 *
 *  @param Lista para agregação.
 *  @return A lista agregada.
*/

GList* joinList (GList *arg) {

	GList *agregada = g_list_alloc();	
	agregada = NULL;

	//So vou agregar se tiver tamanho maior que 1
	if (g_list_length(arg) > 1) {

		GList *aux = NULL;

		int cod, quant; 
		int mont;
		
		char bufferEscrita[MAX_LINE];

		int finalQuant = 0; 
		int finalMont = 0;

		for (aux = arg; aux != NULL; aux = aux -> next) {

			sscanf(aux -> data, "%d %d %d", &cod, &quant, &mont);

			finalQuant += quant;
		}

		finalMont = finalQuant * mont; 

		sprintf(bufferEscrita, "%d %d %d\n", cod, finalQuant, finalMont);
	
		agregada = g_list_prepend(agregada, strdup(bufferEscrita));

	} else agregada = arg;


	return agregada;
}

//*************************************************************************************

/** @brief Função que efetua a agregação de toda a hashtable de Vendas (referenciadas por código)
 *
 *  @param Hashtable para agregação.
 *  @return A hashtable agregada.
*/

GHashTable* agregarList (GHashTable *vendasTable) {

	//Criar nova hashtable
	GHashTable* new = g_hash_table_new(g_str_hash, g_str_equal);

	//Lista de keys e values da hashtable
	GList *keys = NULL, *vals = NULL;

	keys = g_hash_table_get_keys(vendasTable);
	vals = g_hash_table_get_values(vendasTable);

	//Apontadores para percorrer as listas keys e values
	GList *lKey = NULL, *lVal = NULL;

	//Percorrer as listas
 	for (lKey = keys, lVal = vals; 
 		 lKey != NULL && lVal != NULL; 
 		 lKey = lKey -> next, lVal = lVal -> next) {

 		GList *nova = NULL;

 		nova = joinList(lVal -> data);

		g_hash_table_insert(new, 
					        strdup(lKey -> data), 
					        nova);

	}

	return new;
}

//*************************************************************************************

/** @brief Função que escreve a hashtable no ficheiro final (identificado pela hora de agregação)
 *
 *  @param path nome do ficheiro.
 *  @param arg Hashtable.
 *  @return 0 se tudo correr bem.
*/

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

//*************************************************************************************

/** @brief Main: Utiliza hashtables e listas para gerar o ficheiro de vendas agregado.
 *
 *  @return 0 se tudo correr bem.
*/

int main () {

	//Criar a arvore de vendasTable
	GHashTable *vendasTable = NULL;

	//inicializar a arvore de vendasTable
	vendasTable = g_hash_table_new(g_str_hash, g_str_equal); 

	//--------------------------------------------------------------

	int fd_agregador_log = open("../PipeVendas/registos_agregador.log", O_RDONLY, 0666);

	off_t place_to_seek;

	char seek[MAX_LINE];

	if (fd_agregador_log == -1) 
		place_to_seek = 0;
	else {

		if (read(fd_agregador_log, seek, MAX_LINE) > 0) {
			
			place_to_seek = atoi(seek);
		
		} else place_to_seek = 0;

	}

	close(fd_agregador_log);

	//--------------------------------------------------------------

	//Abrir o ficheiro de vendasTable para leitura
 	int fd_vendasTable = open(PATH_VENDAS, O_RDWR, 0666);

 	lseek(fd_vendasTable, place_to_seek, SEEK_SET);

 	//Valor lido pelo readln
 	int n;

 	//Buffers para guardar os elementos lidos do ficheiro
 	char key[MAX_LINE];
 	char val[MAX_LINE];
 	char buffer[MAX_LINE];
 
 	dup2(fd_vendasTable, 0);

  	//Ler a primeira linha
	n = read(0, buffer, 53);
 	
 	//Ler o resto das linhas
 	do {

 		sscanf(buffer, "%s %s", key, val);

		//Inicializar a lista a NULL
		GList *listaNova = g_list_alloc();
		listaNova = NULL;

		//Caso a key já exista
		if (g_hash_table_contains(vendasTable, key) == TRUE) {

			listaNova = g_hash_table_lookup(vendasTable, key);
		}

		listaNova = g_list_prepend(listaNova, strdup(buffer));

		//Inserir o elemento na hashtable
		g_hash_table_insert(vendasTable, strdup(key), listaNova);

		n = read(0, buffer, 53);
 	
	} while (n > 0);

	int ultimaAgreg;

	ultimaAgreg = lseek(fd_vendasTable, 0, SEEK_END);

	close(fd_vendasTable);

	//--------------------------------------------------------------

	fd_agregador_log = open("../PipeVendas/registos_agregador.log", O_CREAT|O_WRONLY|O_TRUNC, 0666);

	char buffer_ult_agreg[MAX_LINE];
	sprintf(buffer_ult_agreg, "%d\n", ultimaAgreg);

	if (write(fd_agregador_log, buffer_ult_agreg, strlen(buffer_ult_agreg)) != -1);

	close(fd_agregador_log);

	//--------------------------------------------------------------

	//Agregar a hashtable
	GHashTable *agregada = NULL;
	agregada = agregarList(vendasTable);

	//--------------------------------------------------------------

	//Mandar para o ficheiro e stdout
	//hashTable_to_stdout(agregada);

	//Nome do ficheiro é a data e hora atual, vou buscar ao sistema

	char datePath[MAX_LINE];

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	sprintf(datePath, "../FILES/%d-%d-%dT%d:%d:%d", tm.tm_year + 1900, 
										   tm.tm_mon + 1, 
										   tm.tm_mday, 
										   tm.tm_hour, 
										   tm.tm_min, 
										   tm.tm_sec);

	printf("path=%s\n", datePath);

	hashTable_to_ficheiro(datePath, agregada);

	cat_file(datePath);

	return 0;
}
