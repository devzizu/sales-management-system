OS := $(shell uname)
ifeq ($(OS),Linux)
    CFLAGS= -g -D_POSIX_C_SOURCE -Wall -Wextra -pedantic -std=c11 -O2
else
    CFLAGS= -g -Wall -Wextra -pedantic -std=c11 -O2
endif

ODIR=obj
CC=gcc

SOURCES=$(wildcard *.c)
OBJECTS=$(foreach o, $(patsubst %.c, %.o, $(SOURCES)), $(ODIR)/$o)

$(ODIR)/%o :
	$(CC) $(CFLAGS) -c -o $@ $(patsubst %.o, %.c, $(patsubst $(ODIR)/%, %, $@))

main: clean $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o main

clean:
	rm -f $(ODIR)/*.o
