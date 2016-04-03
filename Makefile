CC	?=	gcc
CFLAGS	+=	-c -O2 -DNDEBUG -march=native -fomit-frame-pointer -pedantic \
		-ansi
OBJ	= 	binheap.o list.o vector.o
SRC	=	binheap.c binheap.h list.c list.h vector.c vector.h

debug: CFLAGS = -c -Og -g -Wall -Wstrict-prototypes -pedantic -ansi


all: $(OBJ)

debug: $(OBJ)

docs: $(SRC)
	doxygen Doxyfile

binheap.o: binheap.c binheap.h vector.h
	$(CC) $(CFLAGS) $<

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) $<

list.o: list.c list.h
	$(CC) $(CFLAGS) $<

clean:
	rm -rf *.o docs

