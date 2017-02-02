#include <assert.h>

#include "list.h"

int main(int argc, char *argv[])
{
    struct list uut;

    list_init(&uut);

    assert(list_isempty(&uut));

    return 0;
}
