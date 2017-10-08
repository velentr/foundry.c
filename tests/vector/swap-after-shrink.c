#include <assert.h>
#include <stdlib.h>

#include "vector.h"

int main(int argc, char *argv[])
{
    struct vector v;
    int i = 0;
    int *p;

    vec_init(&v, sizeof(int), 0, realloc);

    vec_push(&v, &i);
    i = 1;
    vec_push(&v, &i);

    vec_shrink(&v);
    vec_swap(&v, 0, 1);

    p = vec_get(&v, 0);
    assert(*p == 1);
    p = vec_get(&v, 1);
    assert(*p == 0);

    vec_destroy(&v);

    return 0;
}
