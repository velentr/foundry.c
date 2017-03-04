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

static struct uut_elem elems[TEST_SIZE];

int cmp(const void *a, const void *b)
{
    const struct uut_elem *_a, *_b;

    _a = containerof(a, struct uut_elem, pe);
    _b = containerof(b, struct uut_elem, pe);

    return _a->n - _b->n;
}

int main(int argc, char *argv[])
{
    struct pheap uut0;
    struct pheap uut1;
    struct uut_elem *prev, *cur;
    size_t i;

    pheap_init(&uut0, cmp);
    pheap_init(&uut1, cmp);

    for (i = 0; i < TEST_SIZE; i++)
    {
        elems[i].n = rand();
        pheap_elem_init(&elems[i].pe);
    }

    for (i = 0; i < TEST_SIZE / 2; i++)
    {
        pheap_push(&uut0, &elems[2*i].pe);
        pheap_push(&uut1, &elems[2*i + 1].pe);
    }

    pheap_merge(&uut0, &uut1);

    assert(pheap_isempty(&uut1));

    prev = containerof(pheap_peek(&uut0), struct uut_elem, pe);
    pheap_pop(&uut0);

    for (i = 1; i < TEST_SIZE; i++)
    {
        assert(!pheap_isempty(&uut0));

        cur = containerof(pheap_peek(&uut0), struct uut_elem, pe);
        pheap_pop(&uut0);

        assert(prev->n <= cur->n);

        prev = cur;
    }

    assert(pheap_isempty(&uut0));

    return 0;
}

