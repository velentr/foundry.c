#include <assert.h>

#include "list.h"
#include "pheap.h"
#include "utils.h"

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
    struct pheap uut;
    struct uut_elem a, b;

    pheap_init(&uut, cmp);
    pheap_elem_init(&a.pe);
    pheap_elem_init(&b.pe);

    a.n = 1;
    b.n = 2;

    pheap_push(&uut, &a.pe);
    pheap_push(&uut, &b.pe);

    assert(!pheap_isempty(&uut));
    assert(pheap_peek(&uut) == &a.pe);

    return 0;
}

