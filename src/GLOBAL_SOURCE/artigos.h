#ifndef _ARTIGOS_
#define _ARTIGOS_

typedef struct artigo *ARTIGO;

ARTIGO initArtigo (ARTIGO novo);

ARTIGO insertArtigo (ARTIGO novo, 
					 char *new_key, 
					 char* new_nome, 
					 double new_price);

void write_artigo_to_stdout (ARTIGO arg);

char* getNome (ARTIGO novo);

double getPreco (ARTIGO novo);

#endif