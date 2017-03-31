CC	?=	gcc
INCLUDE	=	../..
LIBS	=	../..
CFLAGS	+=	-I$(INCLUDE) $(LIBS)/bresenham.o -O0 -g
TESTS	=	e n ne nw s se sw w
BIN	=	$(addsuffix .test,$(TESTS))
SRC	=	$(addsuffix .c,$(TESTS))

all: $(BIN)

%.test: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.test

