#include <stdlib.h>
#include <assert.h>

#include "binheap.h"

#ifndef TEST_SIZE
#define TEST_SIZE 1024
#endif

int cmp(const void *a, const void *b)
{
    return a - b;
}

int main(int argc, char *argv[])
{
    struct binheap uut;
    unsigned long i;

    bheap_init(&uut, cmp, 0, realloc);

    for (i = 0; i < TEST_SIZE; i++)
    {
        bheap_push(&uut, (void *)i);
    }

    assert(bheap_size(&uut) == TEST_SIZE);

    bheap_free(&uut);

    return 0;
}
