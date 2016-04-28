#include <assert.h>

#include "list.h"

#ifndef TEST_SIZE
#define TEST_SIZE 1024
#endif

int main(int argc, char *argv[])
{
    struct list uut;
    struct list_elem mem[TEST_SIZE];
    unsigned i;

    list_init(&uut);

    for (i = 0; i < TEST_SIZE; i++)
    {
        list_pushback(&uut, &mem[i]);
    }

    for (i = 0; i < TEST_SIZE; i++)
    {
        struct list_elem *e = list_popfront(&uut);
        assert(e == &mem[i]);
    }

    return 0;
}
