CFLAGS = -Wall -Wextra -Werror -Wpedantic
CC = clang
ESOURCES = encode.c trie.c word.c io.c
DSOURCES = decode.c trie.c word.c io.c

EOBJECTS = encode.o trie.o word.o io.o 
DOBJECTS = decode.o trie.o word.o io.o

EXEBIN = encode
EXEBIN2 = decode

all: $(EXEBIN) $(EXEBIN2)

$(EXEBIN) : $(EOBJECTS)
	$(CC) -o $(EXEBIN) $(EOBJECTS) -lm

$(EOBJECTS) : $(ESOURCES)
	$(CC) -c $(CFLAGS) $(ESOURCES)

$(EXEBIN2) : $(DOBJECTS)
	$(CC) -o $(EXEBIN2) $(DOBJECTS) -lm

$(DOBJECTS) : $(DSOURCES)
	$(CC) -c $(CFLAGS) $(DSOURCES)

clean:
	rm -rf *.o encode decode trie word io code infer-out

infer:
	make clean; infer-capture -- make; infer-analyze -- make
