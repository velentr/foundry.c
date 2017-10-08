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
    const int *_a, *_b;

    _a = a;
    _b = b;

    return *_a - *_b;
}

int main(int argc, char *argv[])
{
    struct binheap uut;
    unsigned i;
    int j;
    int prev, cur;

    binheap_init(&uut, cmp, sizeof(int), 0, realloc);
    srand(TEST_SEED);

    for (i = 0; i < TEST_SIZE; i++)
    {
        j = rand() % TEST_VALUE;
        binheap_push(&uut, &j);
    }

    prev = *(int *)binheap_peek(&uut);
    for (i = 0; i < TEST_SIZE; i++)
    {
        assert(binheap_len(&uut) == TEST_SIZE - i);

        cur = *(int *)binheap_peek(&uut);
        binheap_pop(&uut);

        assert(cur >= prev);

        prev = cur;
    }

    assert(binheap_isempty(&uut));

    binheap_destroy(&uut);

    return 0;
}
