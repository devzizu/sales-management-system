#ifndef _GLOBAL_
#define _GLOBAL_

#define PATH_ARTIGOS "../FILES/ARTIGOS.txt"
#define PATH_STRINGS "../FILES/STRINGS.txt"
#define PATH_STOCK "../FILES/STOCK.txt"
#define PATH_VENDAS "../FILES/VENDAS.txt"
#define PATH_ERRORLOG "../FILES/ERRORLOG.log"

#define PATH_TMP_ARTIGOS "FILES/tmpArtigos.txt"
#define PATH_TMP_STOCK "FILES/tmpStock.txt"

#define MAX_LINE 150
#define LINE_ARTIGOS 22
#define TAM_PEDIDO 25
#define TAM_RESPOSTA 43

double trash_ammount_in_file (char *path);

double is_number_float (char* string);

int is_number_int (char* string);

int inserirArtigo (char* nome, double preco);

int linhasFicheiro (char *path);

char** tokenizeComando (char* string);

char** tokenizeArtigo (char** campos, char* string);

char** tokenizeComandoCV (char* string);

int nr_spaces_in_string (char *string);

size_t readline (int fd, char* buffer, size_t nbyte);

size_t readln (int fd, char *buf, size_t max);

char** tokenizeComandoCV (char* string);

char** tokenizePedidodServidor (char *pedido);

int anySpaceInString (char *string);

size_t readNbytesOnce (int fd, char* buf, size_t nr_bytes);

#endif