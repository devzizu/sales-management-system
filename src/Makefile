#Makefile para compilar o projeto todo

CFLAGS= -O2
CC=gcc

MA = ma_src/
SV = sv_src/
AG = ag_src/
COMP = compactador_src/

all: global ma sv ag comp

global: GLOBAL_SOURCE/global.c GLOBAL_SOURCE/global.h
	$(CC) $(CFLAGS) -c GLOBAL_SOURCE/global.c
	mkdir -p obj
	mv global.o obj

ma:
	cd $(MA) && mkdir -p obj && $(MAKE)
	cd ..
	echo "Manutenção de artigos compiled..."

sv:
	cd $(SV) && mkdir -p obj && $(MAKE)
	cd ..
	echo "Servidor de vendas compiled..."

ag:
	cd $(AG) && mkdir -p obj && $(MAKE)
	cd ..
	echo "Agregador compiled..."

comp:
	cd $(COMP) && mkdir -p obj && $(MAKE)
	cd ..
	echo "Compactador compiled..."
