#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "blkalloc.h"

int main(int argc, char *argv[])
{
    struct blkalloc a;
    void *blk1, *blk2;
    int rc;

    rc = blkalloc_init(&a, malloc, free, 4 * sizeof(void*), 256);
    assert(rc == 0);
    blk1 = blkalloc_alloc(&a);
    memset(blk1, 0, 4 * sizeof(void*));
    blk2 = blkalloc_alloc(&a);
    memset(blk2, 0xFF, 4 * sizeof(void*));

    assert(blk1 + 4*sizeof(void*) <= blk2 || blk2 + 4*sizeof(void*) <= blk1);
    blkalloc_destroy(&a);

    return 0;
}
