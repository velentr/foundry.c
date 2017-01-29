CC	?=	gcc
INCLUDE	=	../..
LIBS	=	../..
CFLAGS	+=	-I$(INCLUDE) $(LIBS)/list.o -O0 -g
TESTS	=	check-empty empty-iter add-one add-two insert-three iter \
		stack-front stack-back queue
BIN	=	$(addsuffix .test,$(TESTS))
SRC	=	$(addsuffix .c,$(TESTS))

all: $(BIN)

%.test: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.test

