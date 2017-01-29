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
    int i;
    long prev;

    bheap_init(&uut, cmp, sizeof(int), 0, realloc);

    i = 3;
    bheap_push(&uut, &i);
    i = 2;
    bheap_push(&uut, &i);
    i = 1;
    bheap_push(&uut, &i);

    assert(*(int*)bheap_peek(&uut) == 1);
    bheap_pop(&uut);

    i = 4;
    bheap_push(&uut, &i);

    assert(*(int*)bheap_peek(&uut) == 2);
    bheap_pop(&uut);
    assert(*(int*)bheap_peek(&uut) == 3);
    bheap_pop(&uut);
    assert(*(int*)bheap_peek(&uut) == 4);
    bheap_pop(&uut);

    bheap_free(&uut);

    return 0;
}
