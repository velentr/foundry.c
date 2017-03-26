#include <assert.h>
#include <stdlib.h>

#include "rbtree.h"
#include "utils.h"

#ifndef TEST_SIZE
#define TEST_SIZE 1024
#endif

struct uut_node
{
    struct rbnode rbn;
    unsigned n;
};

static int cmp(const void *_a, const void *_b)
{
    struct uut_node *a, *b;

    a = containerof(_a, struct uut_node, rbn);
    b = containerof(_b, struct uut_node, rbn);

    return a->n - b->n;
}

int main(int argc, char *argv[])
{
    struct rbtree tree;
    struct uut_node nodes[TEST_SIZE];
    size_t i;

    rbtree_init(&tree, cmp);

    for (i = 0; i < TEST_SIZE; i++)
    {
        nodes[i].n = rand();
        rbtree_insert(&tree, &nodes[i].rbn);
    }

    return 0;
}
