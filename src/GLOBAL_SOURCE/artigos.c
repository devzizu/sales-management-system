//-------------------------------------------------------------------------------

/** @file artigos.c
*
*	@brief Ficheiro que contem a implementação de uma struct que guarda artigos.
*
*/

//-------------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>

#include "artigos.h"

//-------------------------------------------------------------------------------

/**
* Struct que armazena um artigo (os seus atributos).
*/

struct artigo {
	
	/*@{*/
	char* key; /**< referencia do produto. */

	char* nome; /**< Nome do produto. */
	double preco; /**< Preco do produto. */
	int stock; /**< Quantidade de stock do produto. */

	int pos; /**< Codigo real do produto. */

	/*@}*/
};

//-------------------------------------------------------------------------------

ARTIGO initArtigo (ARTIGO novo) {

	novo = (ARTIGO) malloc(sizeof(struct artigo));

	return novo;
}

ARTIGO insertArtigo (ARTIGO novo, 
					 char *new_key, 
					 char* new_nome, 
					 double new_price,
					 int new_pos,
					 int new_stock) {

	novo -> key   = strdup(new_key);
	novo -> nome  = strdup(new_nome);
	novo -> preco = new_price;  
	novo -> pos   = new_pos;
	novo -> stock = new_stock;

	return novo;
}

char* getNome (ARTIGO novo) {

	return novo -> nome;
}

double getPreco (ARTIGO novo) {

	return novo -> preco;
}

char* getKey (ARTIGO novo) {

	return novo -> key;
}

int getPos (ARTIGO novo) {

	return novo -> pos;
}

int getStock (ARTIGO novo) {

	return novo -> stock;
}

int artigo_compare (gconstpointer a, gconstpointer b) {

	int r;

	ARTIGO a_new = (ARTIGO) a; 
	ARTIGO b_new = (ARTIGO) b;

	if (getPos(a_new) == getPos(b_new)) r = 0;
	else if (getPos(a_new) > getPos(b_new)) r = 1;
	else r = -1;

	return r;
}