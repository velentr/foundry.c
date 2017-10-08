#include <stdlib.h>
#include <assert.h>

#include "binheap.h"

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

    binheap_init(&uut, cmp, sizeof(int), 0, realloc);

    assert(binheap_isempty(&uut));

    binheap_destroy(&uut);

    return 0;
}

