CC = gcc -g -Ofast -march=native -Wall
CGLAGS = -I
DEPS = $(wildcard *.h)
OBJ = $(patsubst %.c,%.o,$(wildcard *.c))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) -lm
	
ma: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm

clean:
	rm ma *.o