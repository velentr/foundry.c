#include <stdlib.h>
#include <assert.h>

#include "binheap.h"

#ifndef TEST_SIZE
#define TEST_SIZE 5
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

    binheap_init(&uut, cmp, sizeof(int), TEST_SIZE, realloc);

    assert(binheap_space(&uut) == TEST_SIZE);
    assert(binheap_isempty(&uut));

    binheap_free(&uut);

    return 0;
}
