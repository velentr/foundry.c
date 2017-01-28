#include <stdlib.h>
#include <assert.h>

#include "binheap.h"

int cmp(const void *a, const void *b)
{
    return a - b;
}

int main(int argc, char *argv[])
{
    struct binheap uut;

    bheap_init(&uut, cmp, 0, realloc);

    bheap_push(&uut, NULL);

    assert(bheap_size(&uut) == 1);
    assert(!bheap_isempty(&uut));

    bheap_free(&uut);

    return 0;
}