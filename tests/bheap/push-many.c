#include <stdlib.h>
#include <assert.h>

#include "binheap.h"

#ifndef TEST_SIZE
#define TEST_SIZE 1024
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
    unsigned long i;

    bheap_init(&uut, cmp, sizeof(int), 0, realloc);

    for (i = 0; i < TEST_SIZE; i++)
    {
        bheap_push(&uut, &i);
    }

    assert(bheap_len(&uut) == TEST_SIZE);

    bheap_free(&uut);

    return 0;
}
