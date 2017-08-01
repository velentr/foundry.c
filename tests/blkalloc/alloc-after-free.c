#include <assert.h>
#include <stdlib.h>

#include "blkalloc.h"

int main(int argc, char *argv[])
{
    struct blkalloc a;
    void *block;
    int rc;

    rc = blkalloc_init(&a, malloc, free, 4 * sizeof(void*), 256);
    assert(rc == 0);
    block = blkalloc_alloc(&a);
    blkalloc_free(&a, block);
    block = blkalloc_alloc(&a);
    blkalloc_destroy(&a);

    return 0;
}
