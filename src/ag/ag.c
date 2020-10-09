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
#include <sys/wait.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>
#include <gmodule.h>

#include <time.h>
#include <math.h>

#include "../GLOBAL_SOURCE/global.h"

#define LINE_VENDAS 53

//-------------------------------------------------------------------------------

/** @brief Função que agrega a lista de vendas com a mesma referencia.
 *         Acumula os montantes de todas as vendas e a quantidade vendida numa só venda.
 *
 *  @param Lista para agregação.
 *  @return A lista agregada.
*/

GList* joinList (GList *arg, int flag) 
{
	GList *agregada = g_list_alloc();	
	agregada = NULL;

	//So vou agregar se tiver tamanho maior que 1
	//if (g_list_length(arg) > 1) {

		GList *aux = NULL;

		int cod;
		double quant, mont;
		
		char bufferEscrita[MAX_LINE];

		double finalQuant = 0; 
		double finalMont = 0;

		for (aux = arg; aux != NULL; aux = aux -> next) {

			sscanf(aux -> data, "%d %lf %lf", &cod, &quant, &mont);

			finalQuant += quant;
			finalMont += mont;
		} 

		if(!flag) //20 digitos(forks agreg)
			sprintf(bufferEscrita, "%010d %020.0lf %020.0lf\n", cod, finalQuant, finalMont);
		else // ag final
			sprintf(bufferEscrita, "%d %.0lf %.0lf\n", cod, finalQuant, finalMont);

		agregada = g_list_prepend(agregada, strdup(bufferEscrita));

	//} else agregada = arg;


	return agregada;
}

//*************************************************************************************

/** @brief Função que efetua a agregação de toda a hashtable de Vendas (referenciadas por código)
 *
 *  @param Hashtable para agregação.
 *  @return A hashtable agregada.
*/

GHashTable* agregarList (GHashTable *vendasTable, int flag) {

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

 		nova = joinList(lVal -> data, flag);

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

int hashTable_to_ficheiro (char *path, GHashTable *arg) 
{
	//Sucesso
	int r = 0;

	//Lista de keys e values da hashtable
	GList *vals = NULL;

	vals = g_hash_table_get_values(arg);

	//Apontador para percorrer as listas de values
	GList *lVal = NULL, *l = NULL;

	//Abrir o ficheiro path para escrita
	int fd_vendas = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0666);

	for (lVal = vals; lVal != NULL; lVal = lVal -> next) 
	{	
		for (l = lVal -> data; l != NULL; l = l -> next) 
		{
			if (write(fd_vendas, l -> data, strlen(l -> data)) == -1) 
				r = 1;
		}
	}

	close(fd_vendas);

	return r;
}

// ------------------------------------------------------


int agrForks(int linhaInicio, int linhaMaxima, int nFork, char* file, int flag)
{
	//Criar a arvore de vendasTable
	GHashTable *vendasTable = NULL;

	//inicializar a arvore de vendasTable
	vendasTable = g_hash_table_new(g_str_hash, g_str_equal); 

	//--------------------------------------------------------------


	//Abrir o ficheiro de vendasTable para leitura
 	int fd_vendasTable = open(PATH_VENDAS, O_RDWR, 0666);

 	lseek(fd_vendasTable, (linhaInicio*LINE_VENDAS) - LINE_VENDAS, SEEK_SET);

 	//Valor lido pelo read
 	int n;

 	//Buffers para guardar os elementos lidos do ficheiro
 	char key[MAX_LINE];
 	char val[MAX_LINE];
 	char buffer[MAX_LINE];
 
 	dup2(fd_vendasTable, 0);
 
  	//Ler a primeira linha
	n = read(fd_vendasTable, buffer, LINE_VENDAS);
 	
 	int contador = 0;

 	//Ler o resto das linhas
 	while(contador <= (linhaMaxima - linhaInicio) && n > 0)
 	{
 		sscanf(buffer, "%s %s", key, val);

		//Inicializar a lista a NULL
		GList *listaNova = g_list_alloc();
		listaNova = NULL;

		//Caso a key já exista
		if (g_hash_table_contains(vendasTable, key) == TRUE) 
		{
			listaNova = g_hash_table_lookup(vendasTable, key);
		}

		listaNova = g_list_prepend(listaNova, strdup(buffer));

		//Inserir o elemento na hashtable
		g_hash_table_insert(vendasTable, strdup(key), listaNova);

 		n = read(fd_vendasTable, buffer, LINE_VENDAS);
 		
 		contador++;
	}

	//--------------------------------------------------------------

	//Agregar a hashtable
	GHashTable *agregada = NULL;
	agregada = agregarList(vendasTable, flag);

	//--------------------------------------------------------------
	char Path[MAX_LINE];

	time_t t;
	struct tm tm;   

	switch(flag)
	{
		case 0: //Nome do ficheiro consoante o processo recebido
			sprintf(Path, "../FILES/fork%d", nFork);

			printf("path = %s\n", Path);

			hashTable_to_ficheiro(Path, agregada);

			break;
	
		case 1: //Nome do ficheiro é a data e hora atual, vou buscar ao sistema
			t=  time(NULL);
			tm = *localtime(&t);

			sprintf(Path, "../FILES/%d-%d-%dT%d:%d:%d", tm.tm_year + 1900, 
												   tm.tm_mon + 1, 
												   tm.tm_mday, 
												   tm.tm_hour, 
												   tm.tm_min, 
												   tm.tm_sec);

			printf("path depois de fork = %s\n", Path);

			hashTable_to_ficheiro(Path, agregada);

			cat_file(Path);

			break;

			default: break;
	}

	return 0;
}

// -------------------------------------------------------------------------

void mergeFiles(int fd_fork, int nFork)
{
	int n = 1;
	char buffer[MAX_LINE];

	char forkPath[MAX_LINE];

	sprintf(forkPath, "../FILES/fork%d", nFork);

	int file_fork = open(forkPath, O_RDONLY, 0666);

	while(n > 0)
	{
		n = read(file_fork, buffer, LINE_VENDAS); 

		if(n <= 0)
			break;

		if(write(fd_fork, buffer, n) != -1);
	}

	close(file_fork);
}


//*************************************************************************************

/** @brief Main: Utiliza hashtables e listas para gerar o ficheiro de vendas agregado.
 *
 *  @return 0 se tudo correr bem.
*/

int main() 
{
	int fd_agregador_log = open("../PipeVendas/registos_agregador.log", O_RDONLY, 0666);

	off_t place_to_seek;

	char seek[MAX_LINE];

	if (fd_agregador_log == -1) place_to_seek = 0;
 	else {
	
		readln(fd_agregador_log, seek, MAX_LINE); 	

		place_to_seek = atoi(seek);

 	}

	int fd_vendasTable = open(PATH_VENDAS, O_RDONLY, 0666);

	int ultimaAgreg = lseek(fd_vendasTable, 0, SEEK_END);

	close(fd_vendasTable);

	close(fd_agregador_log);

	//--------------------------------------------------------------

	fd_agregador_log = open("../PipeVendas/registos_agregador.log", O_CREAT|O_WRONLY | O_TRUNC, 0666);

	char buffer_ult_agreg[MAX_LINE];
	sprintf(buffer_ult_agreg, "%d\n", ultimaAgreg);

	if(write(fd_agregador_log, buffer_ult_agreg, strlen(buffer_ult_agreg)) != -1);

	close(fd_agregador_log);

	int nbytesAg = ultimaAgreg - place_to_seek;

	int nlinhas = nbytesAg/LINE_VENDAS;

	if(!nlinhas)
		return 0;  // NÃO É CRIADO DATEPATH QUANDO NÃO HÁ NADA PARA AGREGAR

	int i, nforks;

	int file_agr = open("../FILES/file_agr", O_CREAT | O_WRONLY | O_APPEND, 0666);

	for(i = 0, nforks = 1; i < nlinhas; i += 5000, nforks++)
	{
		if(fork() == 0)
		{
			if(nlinhas >= 5000 + i)
				agrForks(i + 1, i + 5000, nforks, "", 0);
			else
				agrForks(i + 1, nlinhas, nforks, "", 0);

			mergeFiles(file_agr, nforks);
			
			_exit(0);
		}
	}

	for(i = 0; i < nforks; i++)
		wait(NULL);

	close(file_agr);

	agrForks(1, nlinhas, 0, "../FILES/file_agr", 1);

	char forkPath[MAX_LINE];

	for(i = 0; i < nforks; i++)
	{
		sprintf(forkPath, "../FILES/fork%d", i);
		if(remove(forkPath) != -1);		
	}	

	if(remove("../FILES/file_agr") != -1);

	return 0;
}
