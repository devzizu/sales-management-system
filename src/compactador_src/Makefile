OS := $(shell uname)
ifeq ($(OS),Linux)
    CFLAGS= `pkg-config --cflags libxml-2.0` `pkg-config --cflags glib-2.0` -g -O2
else
    CFLAGS= -g -pedantic -std=c11 -O2
endif

LIBS = `pkg-config --libs libxml-2.0` `pkg-config --libs glib-2.0`
ODIR=obj
CC=gcc

SOURCES=$(wildcard *.c)
OBJECTS=$(foreach o, $(patsubst %.c, %.o, $(SOURCES)), $(ODIR)/$o)

print-% : ; @echo $* = $($*)

$(ODIR)/%o :
	$(CC) $(CFLAGS) -c -o $@ $(patsubst %.o, %.c, $(patsubst $(ODIR)/%, %, $@))

compactador: clean $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o compactador $(LIBS)

clean:
	rm -f $(ODIR)/*.o