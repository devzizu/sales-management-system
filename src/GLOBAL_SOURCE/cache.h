#ifndef _CACHE_
#define _CACHE_

typedef struct celula *CELULA;

typedef struct cache *CACHE;

CACHE init_cache (int size);

CELULA init_celula (CELULA c, int cod, int prec);

CACHE add_cache (CACHE c, CELULA nova, int *pos_if_exists);

int lookup_code_cache (CACHE c, int codigo_procura);

CACHE update_cache (CACHE c, int *pos, CELULA nova);

void print_cache (CACHE c);

CACHE setPreco (CACHE c, int pos, int novo_preco);

int getPreco (CACHE c, int posicao);

#endif