#include <assert.h>

#include "list.h"

int main(int argc, char *argv[])
{
    struct list uut;
    struct list_elem a, b, c;
    struct list_elem *e;

    list_init(&uut);

    list_pushback(&uut, &a);
    list_insert(&a, &b);
    list_insert(&b, &c);

    e = list_begin(&uut);
    assert(e == &a);
    e = list_next(e);
    assert(e == &b);
    e = list_next(e);
    assert(e == &c);
    e = list_next(e);
    assert(e == list_end(&uut));

    list_remove(&b);
    assert(list_next(&a) == &c);
    assert(list_prev(&c) == &a);

    list_remove(&c);
    assert(list_next(&a) == list_end(&uut));
    assert(list_prev(&a) == list_end(&uut));

    list_remove(&a);
    assert(list_isempty(&uut));

    return 0;
}
