#include <stdlib.h>
#include <assert.h>

#include "binheap.h"

#ifndef TEST_SIZE
#define TEST_SIZE 5
#endif

int cmp(const void *a, const void *b)
{
    return a - b;
}

int main(int argc, char *argv[])
{
    struct binheap uut;

    bheap_init(&uut, cmp, TEST_SIZE * BHEAP_ELEMSIZE, realloc);

    assert(bheap_space(&uut) == TEST_SIZE);
    assert(bheap_isempty(&uut));

    bheap_free(&uut);

    return 0;
}
