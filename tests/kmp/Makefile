CC	?=	gcc
INCLUDE	=	../..
LIBS	=	../..
CFLAGS	+=	-I$(INCLUDE) $(LIBS)/kmp.o -O0 -g
TESTS	=	backtrack empty-key empty-string key-at-end key-at-start \
		key-in-middle length-1-fail length-1-pass length-2-fail \
		length-2-pass not-found partial-match too-long
BIN	=	$(addsuffix .test,$(TESTS))
SRC	=	$(addsuffix .c,$(TESTS))

all: $(BIN)

%.test: %.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.test

