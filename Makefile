# Makefile
 
FILES   = lex.c parse.c stmt.c terp.c
CC      = gcc
CFLAGS  = 
 
terp: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o terp
 
lex.c: lex.l 
	flex lex.l
 
parse.c: parse.y lex.c
	bison parse.y

debug: $(FILES)
	$(CC) $(CFLAGS) -g $(FILES) -o terp

clean:
	rm -f *.o *~ lex.c lex.h parse.c parse.h terp
