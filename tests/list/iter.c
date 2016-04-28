#include <assert.h>

#include "list.h"

#ifndef TEST_SIZE
#define TEST_SIZE 1024
#endif

int main(int argc, char *argv[])
{
    struct list uut;
    struct list_elem mem[TEST_SIZE];
    struct list_elem *e;
    unsigned i;

    list_init(&uut);

    for (i = 0; i < TEST_SIZE; i++)
    {
        list_pushback(&uut, &mem[i]);
    }

    i = 0;
    for (e = list_begin(&uut); e != list_end(&uut); e = list_next(e))
    {
        assert(e == &mem[i]);
        i++;
    }

    assert(i == TEST_SIZE);

    return 0;
}
