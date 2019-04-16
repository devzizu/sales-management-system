#ifndef _GLOBAL_
#define _GLOBAL_

#define PATH_ARTIGOS "FILES/ARTIGOS.txt"
#define PATH_STRINGS "FILES/STRINGS.txt"

#define PATH_TMP_ARTIGOS "FILES/tmpArtigos.txt"

#define MAX_LINE 150

int linhasFicheiro (char *path);

char** tokenizeComando (char* string);

char** tokenizeArtigo (char** campos, char* string);

size_t readline (int fd, char* buffer, size_t nbyte);

size_t readln (int fd, char *buf, size_t max);

#endif