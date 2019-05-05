//-------------------------------------------------------------------------------

/** @file global.h
*
*	@brief Header do global.c contém funções auxiliares aos programas.
*
*/

//-------------------------------------------------------------------------------

#ifndef _GLOBAL_
#define _GLOBAL_

//-------------------------------------------------------------------------------

/*PATHS para os ficheiros principais do programa*/

#define PATH_ARTIGOS "../FILES/ARTIGOS"
#define PATH_STRINGS "../FILES/STRINGS"
#define PATH_STOCK "../FILES/STOCK.txt"
#define PATH_VENDAS "../FILES/VENDAS"

/*Ficheiro onde sao encaminhados os erros de execução dos programas*/
#define PATH_ERRORLOG "../FILES/ERRORLOG.log"

#define PATH_TMP_ARTIGOS "FILES/tmpArtigos.txt"
#define PATH_TMP_STOCK "FILES/tmpStock.txt"

//-------------------------------------------------------------------------------

#define MAX_LINE 150

#define TAM_PEDIDO 29
#define TAM_RESPOSTA 42

#define LINE_ARTIGOS 22
#define LINE_STOCK 11

//-------------------------------------------------------------------------------

#define BASE_PATH "../PipeVendas/pipePrintCliente"

//-------------------------------------------------------------------------------

/** @brief Verifica se uma string contem um inteiro e caso tenha retorna-o
 *
 *  @param string para parse.
 *  @return -1 caso nao seja inteiro.
*/

int is_number_int (char* string);

//-------------------------------------------------------------------------------

/** @brief Função que insere um artigo no fim do ficheiro artigos.
 *
 *  @param nome string com o nome do artigo.
 *  @param stock quantidade de stock.
 *  @param preco preco do artigo.
 *
 *  @return 0.
*/

int inserirArtigo (char* nome, int preco, int stock);

//-------------------------------------------------------------------------------

/** @brief Calcula o numero de linhas de um ficheiro
 *
 *  @param path nome do ficheiro.
 *  @return o numero de linhas.
*/

int linhasFicheiro (char *path);

//-------------------------------------------------------------------------------

char** tokenizeComando (char* string);

//-------------------------------------------------------------------------------

/** @brief Cria um array de pointers para linha artigo e guarda no argumento
 *
 *  @param campos array de pointers.
 *  @param pedido string.
 *  @return array de pointers.
*/

char** tokenizeArtigo (char** campos, char* string);

//-------------------------------------------------------------------------------

/** @brief Cria um array de pointers para strings com 1 espaço
 *
 *  @param string string.
 *  @return array de pointers.
*/

char** tokenizeComandoCV (char* string);

//-------------------------------------------------------------------------------

/** @brief Calcula o numero de espaços numa string
 *
 *  @param string string.
 *  @return numero de espaços.
*/

int nr_spaces_in_string (char *string);

//-------------------------------------------------------------------------------

/** @brief Readline de caracter a caracter
 *
 *  @param fd descriptor de onde ler.
 *  @param buffer resultado da leitura (string lida).
 *  @return numero de bytes lidos.
*/

size_t readline (int fd, char* buffer, size_t nbyte);

//-------------------------------------------------------------------------------

/** @brief Readline de caracter a caracter (2ª implementação)
 *
 *  @param fd descriptor de onde ler.
 *  @param buffer resultado da leitura (string lida).
 *  @return max numero de bytes lidos.
*/

size_t readln (int fd, char *buf, size_t max);

//-------------------------------------------------------------------------------

/** @brief Cria um array de pointers para strings com 2 espaços (para comandos pedidos)
 *
 *  @param pedido string.
 *  @return array de pointers.
*/

char** tokenizeComandoCV (char* string);

//-------------------------------------------------------------------------------

/** @brief Cria um array de pointers para strings com 2 espaços
 *
 *  @param pedido string.
 *  @return array de pointers.
*/

char** tokenizePedidodServidor (char *pedido);

//-------------------------------------------------------------------------------

/** @brief Read de n bytes de uma vez
 *
 *  @param fd descriptor de onde ler.
 *  @param buffer resultado da leitura (string lida).
 *  @return numero de bytes desejados.
*/

size_t readNbytesOnce (int fd, char* buf, size_t nr_bytes);

//-------------------------------------------------------------------------------

/** @brief Executa o comando cat da bash
 *
 *  @param file_to_cat ficheiro.
 *  @return.
*/

int cat_file (char *file_to_cat);

#endif