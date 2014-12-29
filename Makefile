CC	?=	gcc
CFLAGS	=	-O2 -c -Wall -Wstrict-prototypes -pedantic -ansi
OBJ	= 	binheap.o vector.o

all: $(OBJ)

binheap.o: binheap.c binheap.h vector.h
	$(CC) $(CFLAGS) $<

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o

