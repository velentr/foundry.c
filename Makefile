CC	?=	gcc
CFLAGS	=	-O2 -c -Wall -Wstrict-prototypes -pedantic -ansi
OBJ	= vector.o

all: $(OBJ)

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o

