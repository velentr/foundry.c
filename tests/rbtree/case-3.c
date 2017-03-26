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
    struct uut_node a;
    struct uut_node b;
    struct uut_node c;
    struct uut_node d;

    a.n = 1;    /* root */
    b.n = 2;    /* parent */
    c.n = 0;    /* uncle */
    d.n = 3;    /* uut */

    rbtree_init(&tree, cmp);
    rbtree_insert(&tree, &a.rbn);
    rbtree_insert(&tree, &b.rbn);
    rbtree_insert(&tree, &c.rbn);
    rbtree_insert(&tree, &d.rbn);

    return 0;
}
