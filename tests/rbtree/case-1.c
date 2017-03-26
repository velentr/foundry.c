#include <assert.h>

#include "rbtree.h"
#include "utils.h"

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
    struct uut_node node;

    rbtree_init(&tree, cmp);
    rbtree_insert(&tree, &node.rbn);

    return 0;
}
