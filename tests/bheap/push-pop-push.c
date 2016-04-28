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
    unsigned i;
    long prev;

    bheap_init(&uut, cmp, 0, realloc);

    bheap_push(&uut, (void *)3);
    bheap_push(&uut, (void *)2);
    bheap_push(&uut, (void *)1);

    assert(bheap_pop(&uut) == (void *)1);

    bheap_push(&uut, (void *)4);

    assert(bheap_pop(&uut) == (void *)2);
    assert(bheap_pop(&uut) == (void *)3);
    assert(bheap_pop(&uut) == (void *)4);

    bheap_free(&uut);

    return 0;
}
