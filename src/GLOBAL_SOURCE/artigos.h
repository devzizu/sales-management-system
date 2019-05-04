#ifndef _ARTIGOS_
#define _ARTIGOS_

#include <glib.h>

typedef struct artigo *ARTIGO;

ARTIGO initArtigo (ARTIGO novo);

ARTIGO insertArtigo (ARTIGO novo, 
					 char *new_key, 
					 char* new_nome, 
					 double new_price,
					 int new_pos,
					 int new_stock);

void write_artigo_to_stdout (ARTIGO arg);

char* getNome (ARTIGO novo);

double getPreco (ARTIGO novo);

char* getKey (ARTIGO novo);

int getPos (ARTIGO novo);

int getStock (ARTIGO novo);

int artigo_compare (gconstpointer a, gconstpointer b);

#endif