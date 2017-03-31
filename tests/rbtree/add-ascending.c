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

static int _check_prev(const struct rbnode *_node, void *_prev)
{
    int *prev = _prev;
    const struct uut_node *node = containerof(_node, struct uut_node, rbn);

    assert(node->n >= *prev);
    *prev = node->n;

    return 0;
}

int main(int argc, char *argv[])
{
    struct rbtree tree;
    struct uut_node nodes[TEST_SIZE];
    size_t i;
    int prev;

    rbtree_init(&tree, cmp);

    for (i = 0; i < TEST_SIZE; i++)
    {
        nodes[i].n = i;
        rbtree_insert(&tree, &nodes[i].rbn);
    }

    prev = 0;

    rbtree_traverse(&tree, _check_prev, &prev);

    return 0;
}

