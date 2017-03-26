CC	?=	gcc
INCLUDE	=	../..
LIBS	=	../..
CFLAGS	+=	-I$(INCLUDE) $(LIBS)/list.o $(LIBS)/pheap.o -O0 -g
TESTS	=	add-many add-one add-two-a add-two-b check-empty merge
BIN	=	$(addsuffix .test,$(TESTS))
SRC	=	$(addsuffix .c,$(TESTS))

all: $(BIN)

%.test: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.test

