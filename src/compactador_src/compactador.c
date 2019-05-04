#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "../GLOBAL_SOURCE/global.h"
#include "../GLOBAL_SOURCE/artigos.h"

double lixo_strings () {

	int max_reference = 0, total_lines = 0;

	double total_trash = 0.0; //% of trash

	int fd_artigo = open(PATH_ARTIGOS, O_RDONLY, 0666);

 	//Linha lida
	char buffer[MAX_LINE];
	//Referencia na linha
	char *key_ref;
	//Preco lido
	char *preco;
	int ref_atual;

	//Ler a primeira linha
	int n = read(fd_artigo, buffer, LINE_ARTIGOS);
 	
 	//Ler o resto das linhas
 	do {

 		//Sacar o preco e referencia (para o ficheiro strings)
 		key_ref = strdup(strtok(buffer, " "));
 		preco   = strdup(strtok(NULL  , " "));

 		ref_atual = atoi(key_ref);

 		if (ref_atual > max_reference)
 			max_reference = ref_atual;

	 	n = read(fd_artigo, buffer, LINE_ARTIGOS);
		
 		total_lines++;

	} while (n > 0);

	close(fd_artigo);

	total_trash = 1.0 - ((double) total_lines / max_reference);

	return total_trash;
}

char* get_line_from_file (char *path, int line_to_search) {

	int fd_file = open(path, O_RDONLY, 0666);

	int n = 0, curr_line = 1, r = 0;
 	
 	//Linha lida
	char buffer[MAX_LINE];

	do {

 		n = readln(fd_file, buffer, MAX_LINE);
		
 		if (curr_line == line_to_search) {
 			r = 1;
 			break;
 		} 

 		curr_line++;

	} while (n > 0); 	

	char *res;

	if (!r) 
		res = NULL;
	else { 
		res = strdup(buffer);
	}
	
	return res;
}

int get_stock_from_file (int referencia) {

	int fd_stock, pos_leitura, n;
	off_t offset;
	
	fd_stock = open(PATH_STOCK, O_RDONLY, 0666);
	pos_leitura = (referencia-1) * LINE_STOCK;
	offset = lseek(fd_stock, pos_leitura, SEEK_SET);

	char oldStock[MAX_LINE];

	n = read(fd_stock, oldStock, LINE_ARTIGOS);

	close(fd_stock);

	return atoi(oldStock);
}

int main(int argc, char const *argv[]) {
	
	printf("Lixo: %.3lf per cent\n", lixo_strings());

	int fd_artigos = open(PATH_ARTIGOS, O_RDONLY, 0666);

	GList *stringsCompact = NULL;

	int n = 0;
 	
 	//Linha lida
	char buffer[MAX_LINE];
	//Referencia na linha
	char *key_ref;
	//Preco lido
	char *preco;

	int key_int;

	key_int = 0;

 	//Ler a primeira linha
	n = read(fd_artigos, buffer, LINE_ARTIGOS);
 	
	key_int++;

 	//Ler o resto das linhas
 	do {

 		ARTIGO novo = initArtigo(novo);

 		//Sacar o preco e referencia (para o ficheiro strings)
 		key_ref = strdup(strtok(buffer, " "));
 		preco   = strdup(strtok(NULL  , " "));

 		novo = insertArtigo(novo, key_ref, 
 								  get_line_from_file(PATH_STRINGS, atoi(key_ref)),
 								  atof(preco),
 								  key_int,
 								  get_stock_from_file(key_int));

 		//write_artigo_to_stdout(novo);
 		
 		//Inserir o artigo na hashtable
 		stringsCompact = g_list_prepend(stringsCompact, novo);

 		key_int++;

 		n = read(fd_artigos, buffer, LINE_ARTIGOS);
 	
	} while (n > 0);

	close(fd_artigos);

	//-------------------------------------------------------------------

	//Limpar o ficheiro de artigos
	fd_artigos = open(PATH_ARTIGOS, O_CREAT|O_TRUNC|O_WRONLY, 0666);
	close(fd_artigos);

	//Limpar o ficheiro de strings
	int fd_strings = open(PATH_STRINGS, O_CREAT|O_TRUNC|O_WRONLY, 0666);
	close(fd_strings);

	//Limpar o ficheiro de stocks
	int fd_stock = open(PATH_STOCK, O_CREAT|O_TRUNC|O_WRONLY, 0666);
	close(fd_stock);

	GList *values = NULL, *auxValues = NULL;

	stringsCompact = g_list_sort(stringsCompact, artigo_compare);

	for (auxValues = stringsCompact; auxValues != NULL; auxValues = auxValues -> next) {

		char *nome_art = strdup(getNome(auxValues -> data));
		nome_art[strlen(nome_art)-1]='\0';

		inserirArtigo(nome_art, getPreco(auxValues -> data), getStock(auxValues -> data));
	}

	printf("Reducing the ammount of trash in file...\n");
	printf("Lixo: %.3lf per cent\n", lixo_strings());

	return 0;
}