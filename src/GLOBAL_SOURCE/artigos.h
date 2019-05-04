//-------------------------------------------------------------------------------

/** @file artigos.h
*
*	@brief Ficheiro que contem as definiçoes das funções sobre artigos.
*
*/

//-------------------------------------------------------------------------------

#ifndef _ARTIGOS_
#define _ARTIGOS_

//-------------------------------------------------------------------------------

#include <glib.h>

//-------------------------------------------------------------------------------

/** Type para a struct artigo */
typedef struct artigo *ARTIGO;

//-------------------------------------------------------------------------------

/** @brief Inicializa um artigo, cria memória para o mesmo.
 *
 *  @param novo Apontador para o artigo.
 *  @return o artigo.
*/

ARTIGO initArtigo (ARTIGO novo);

//-------------------------------------------------------------------------------

/** @brief Inicializa um artigo com os seus parametros
 *
 *  @param novo apontador para o artigo. 
 *  @param parametros do artigo.
 *  @return o artigo inicializado.
*/

ARTIGO insertArtigo (ARTIGO novo, 
					 char *new_key, 
					 char* new_nome, 
					 double new_price,
					 int new_pos,
					 int new_stock);

//-------------------------------------------------------------------------------

/** @brief retorna o parametro nome do artigo
 *
 *  @param novo o artigo. 
 *  @return string nome.
*/

char* getNome (ARTIGO novo);

//-------------------------------------------------------------------------------

/** @brief retorna o parametro preco do artigo
 *
 *  @param novo o artigo. 
 *  @return double preco.
*/

double getPreco (ARTIGO novo);

//-------------------------------------------------------------------------------

/** @brief retorna o parametro key do artigo
 *
 *  @param novo o artigo. 
 *  @return a key (referencia do artigo).
*/

char* getKey (ARTIGO novo);

//-------------------------------------------------------------------------------

/** @brief retorna o parametro pos(ição) do artigo
 *
 *  @param novo o artigo. 
 *  @return a pos (codigo do artigo).
*/

int getPos (ARTIGO novo);

//-------------------------------------------------------------------------------

/** @brief retorna o parametro stock do artigo
 *
 *  @param novo o artigo. 
 *  @return stock do artigo.
*/

int getStock (ARTIGO novo);

//-------------------------------------------------------------------------------

/** @brief compara dois artigos pelo seu parametro pos
 *
 *  @param a, b pointers para uma struct. 
 *  @return a>b?1:(a==b?0:-1).
*/

int artigo_compare (gconstpointer a, gconstpointer b);

//-------------------------------------------------------------------------------

#endif