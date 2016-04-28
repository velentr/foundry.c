#include <assert.h>

#include "list.h"

int main(int argc, char *argv[])
{
    struct list uut;
    struct list_elem a, b;
    struct list_elem *e;

    list_init(&uut);

    list_pushback(&uut, &a);
    list_pushback(&uut, &b);

    assert(list_head(&uut) == &a);
    assert(list_tail(&uut) == &b);

    return 0;
}
