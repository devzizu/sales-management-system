OS := $(shell uname)
ifeq ($(OS),Linux)
    CFLAGS= -g -O2
else
    CFLAGS= -g -pedantic -std=c11 -O2
endif

ODIR=obj
CC=gcc

SOURCES=$(wildcard *.c)
OBJECTS=$(foreach o, $(patsubst %.c, %.o, $(SOURCES)), $(ODIR)/$o)

print-% : ; @echo $* = $($*)

$(ODIR)/%o :
	$(CC) $(CFLAGS) -c -o $@ $(patsubst %.o, %.c, $(patsubst $(ODIR)/%, %, $@))

sv: clean $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o sv

clean:
	rm -f $(ODIR)/*.o