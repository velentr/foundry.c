CC	?=	gcc
INCLUDE	=	../..
LIBS	=	../..
CFLAGS	+=	-I$(INCLUDE) $(LIBS)/rbtree.o -O0 -g
TESTS	=	add-ascending add-descending add-many case-1 case-2 case-3
BIN	=	$(addsuffix .test,$(TESTS))
SRC	=	$(addsuffix .c,$(TESTS))

all: $(BIN)

%.test: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.test

