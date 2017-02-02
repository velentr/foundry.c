#include <assert.h>

#include "list.h"

int main(int argc, char *argv[])
{
    struct list uut;
    struct list_elem e;

    list_init(&uut);
    list_pushback(&uut, &e);

    assert(!list_isempty(&uut));
    assert(list_head(&uut) == &e);
    assert(list_tail(&uut) == &e);
    assert(list_popback(&uut) == &e);

    return 0;
}
