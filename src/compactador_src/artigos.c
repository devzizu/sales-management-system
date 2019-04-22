
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "artigos.h"

struct artigo {

	char* key;

	char* nome;
	double preco;

};

ARTIGO initArtigo (ARTIGO novo) {

	novo = (ARTIGO) malloc(sizeof(struct artigo));

	return novo;
}

ARTIGO insertArtigo (ARTIGO novo, 
					 char *new_key, 
					 char* new_nome, 
					 double new_price) {

	novo -> key   = strdup(new_key);
	novo -> nome  = strdup(new_nome);
	novo -> preco = new_price;  

	return novo;
}

void write_artigo_to_stdout (ARTIGO arg) {

	printf("\nkey   : %s\n", arg -> key);
	printf("nome  : %s", arg -> nome);
	printf("preco : %.2lf\n", arg -> preco);

}

char* getNome (ARTIGO novo) {

	return novo -> nome;
}

double getPreco (ARTIGO novo) {

	return novo -> preco;
}