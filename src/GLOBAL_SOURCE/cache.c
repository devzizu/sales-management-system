
#include <stdio.h>
#include <stdlib.h>

#include "cache.h"

struct celula {

	int codigo;
	int preco;

};

struct cache {

	CELULA *cache;

	int next;
	int max;
	int nr_elementos;
};

CACHE init_cache (int size) {

	CACHE nova = (CACHE) malloc(sizeof(struct cache));

	nova -> cache = malloc(sizeof(struct celula) * size);
	nova -> next  = 0;
	nova -> max   = size;
	nova -> nr_elementos = 0;

	return nova;
}

CELULA init_celula (CELULA c, int cod, int prec) {

	c = (CELULA) malloc(sizeof(struct celula));

	c -> codigo = cod;
	c -> preco  = prec;

	return c;
}

CACHE add_cache (CACHE c, CELULA nova, int *pos_if_exists) {

	if (*pos_if_exists == -1) {

		if ((c -> next) == ((c -> max) - 1)) {	
			c -> cache[c -> next] = nova;
			c -> next = 0;
		} else {

			c -> cache[c -> next] = nova;
			(c -> next)++;
		}

		if (c -> nr_elementos < 10)
			(c -> nr_elementos)++;
	}
	
	return c;
}

int lookup_code_cache (CACHE c, int codigo_procura) {

	if (c == NULL) 
		return -1;

	int i = 0;
	for (; i < (c -> nr_elementos); i++) {

		if (c -> cache[i] -> codigo == codigo_procura)
			return i;
	}

	return -1;
}

CACHE update_cache (CACHE c, int *pos, CELULA nova) {

	c -> cache[*pos] = nova;

	return c;
}

CACHE setPreco (CACHE c, int pos, int novo_preco) {

	c -> cache[pos] -> preco = novo_preco;

	return c;
}

int getPreco (CACHE c, int posicao) {

	return c -> cache[posicao] -> preco;
}

void print_cache (CACHE c) {

	printf("\n---------------------------------------------------------\n");
	printf("\n-> Cache com %d elementos...\n\n", c -> nr_elementos);

	int i = 0;
	for (; i < (c -> nr_elementos); i++) {

		printf("[#%d] cod: %d | prec: %d\n", i, 
										   c -> cache[i] -> codigo, 
										   c -> cache[i] -> preco);

	}

	printf(":: next: %d\n", c -> next);
	printf(":: nr_elem: %d\n", c -> nr_elementos);

	printf("\n---------------------------------------------------------\n");
}


void free_cache (CACHE c) {

	free(c -> cache);

}