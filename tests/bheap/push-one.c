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
    int i = 0;

    bheap_init(&uut, cmp, sizeof(int), 0, realloc);

    bheap_push(&uut, &i);

    assert(bheap_len(&uut) == 1);
    assert(!bheap_isempty(&uut));

    bheap_free(&uut);

    return 0;
}
