CC	?=	gcc
CFLAGS	+=	-c -O2 -march=native -fomit-frame-pointer \
		-Wall -Wstrict-prototypes -pedantic -ansi
OBJ	= 	binheap.o list.o vector.o

all: $(OBJ)

binheap.o: binheap.c binheap.h vector.h
	$(CC) $(CFLAGS) $<

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) $<

list.o: list.c list.h
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o

