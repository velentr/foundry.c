#include <assert.h>
#include <stdlib.h>

#include "list.h"
#include "pheap.h"
#include "utils.h"

#ifndef TEST_SIZE
#define TEST_SIZE 1024
#endif /* TEST_SIZE */

struct uut_elem
{
    struct pheap_elem pe;
    int n;
};

int cmp(const void *a, const void *b)
{
    const struct uut_elem *_a, *_b;

    _a = containerof(a, struct uut_elem, pe);
    _b = containerof(b, struct uut_elem, pe);

    return _a->n - _b->n;
}

int main(int argc, char *argv[])
{
    struct uut_elem elems[TEST_SIZE];
    struct uut_elem *cur, *prev;
    struct pheap uut;
    size_t i;

    pheap_init(&uut, cmp);

    for (i = 0; i < TEST_SIZE; i++)
    {
        pheap_elem_init(&elems[i].pe);
        elems[i].n = rand();

        pheap_push(&uut, &elems[i].pe);
    }

    assert(!pheap_isempty(&uut));

    prev = containerof(pheap_peek(&uut), struct uut_elem, pe);
    pheap_pop(&uut);

    for (i = 0; i < TEST_SIZE - 1; i++)
    {
        assert(!pheap_isempty(&uut));
        cur = containerof(pheap_peek(&uut), struct uut_elem, pe);
        pheap_pop(&uut);

        assert(prev->n <= cur->n);

        prev = cur;
    }

    assert(pheap_isempty(&uut));

    return 0;
}

