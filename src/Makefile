#Makefile para compilar o projeto todo

MA = ma_src/
SV = sv_src/

all: ma sv

ma:
	cd $(MA) && mkdir -p obj && $(MAKE)
	cd ..
	echo "Manutenção de artigos compiled..."

sv:
	cd $(SV) && mkdir -p obj && $(MAKE)
	cd ..
	echo "Servidor de vendas compiled..."