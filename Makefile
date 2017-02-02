CC	?=	gcc
MAKE	?=	make
CFLAGS	+=	-c -O3 -DNDEBUG -march=native -pedantic -std=c99 -pipe
CFLAGS	+=	-Wall -Wextra -Werror -Wno-unused-function
MOD	=	binheap bresenham htable list vector
OBJ	= 	$(addsuffix .o,$(MOD))
SRC	=	$(addsuffix .c,$(MOD)) $(addsuffix .h,$(MOD))
TESTDIR	=	tests

.PHONY: debug all mostlyclean distclean clean test

debug: CFLAGS = -c -O0 -g -Wall -Wextra -Wno-unused-function \
    -Wstrict-prototypes -pedantic -std=c99


all: $(OBJ)

test: mostlyclean debug
	$(MAKE) -C $(TESTDIR)
	scripts/testbench.pl $(TESTDIR)/*/*.test

debug: mostlyclean $(OBJ)

docs: $(SRC) Doxyfile README.md
	doxygen Doxyfile

binheap.o: binheap.c binheap.h vector.h
	$(CC) $(CFLAGS) $<

bresenham.o: bresenham.c bresenham.h
	$(CC) $(CFLAGS) $<

htable.o: htable.c htable.h list.h utils.h
	$(CC) $(CFLAGS) $<

list.o: list.c list.h
	$(CC) $(CFLAGS) $<

vector.o: vector.c vector.h
	$(CC) $(CFLAGS) $<

mostlyclean:
	rm -rf *.o

clean: mostlyclean distclean
	$(MAKE) -k clean -C $(TESTDIR)

distclean: mostlyclean
	rm -rf docs
