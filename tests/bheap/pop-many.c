#include <stdlib.h>
#include <assert.h>

#include "binheap.h"

#ifndef TEST_SIZE
#define TEST_SIZE 1024
#endif

#ifndef TEST_SEED
#define TEST_SEED 1234
#endif

#ifndef TEST_VALUE
#define TEST_VALUE 64
#endif

int cmp(const void *a, const void *b)
{
    return a - b;
}

int main(int argc, char *argv[])
{
    struct binheap uut;
    unsigned i;
    long prev;

    bheap_init(&uut, cmp, 0, realloc);
    srand(TEST_SEED);

    for (i = 0; i < TEST_SIZE; i++)
    {
        bheap_push(&uut, (void *)(long)(rand() % TEST_VALUE));
    }

    prev = (long)bheap_peek(&uut);
    for (i = 0; i < TEST_SIZE; i++)
    {
        assert(bheap_size(&uut) == TEST_SIZE - i);

        long peek = (long)bheap_peek(&uut);
        long pop  = (long)bheap_pop(&uut);

        assert(pop == peek);
        assert(peek >= prev);

        prev = peek;
    }

    assert(bheap_isempty(&uut));

    bheap_free(&uut);

    return 0;
}
