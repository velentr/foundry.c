/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary, for any
 * purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the
 * public domain. We make this dedication for the benefit of the public at large
 * and to the detriment of our heirs and successors. We intend this dedication
 * to be an overt act of relinquishment in perpetuity of all present and future
 * rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */


/**
 * \file rbtree.h
 *
 * \brief A red-black self-balancing binary search tree.
 *
 * Each node in the binary tree is labeled with a color, either red or black.
 * Whenever nodes are added or removed, the tree is rebalanced and recolored. A
 * few simple rules for the rebalancing and recoloring are used to preserve an
 * almost balanced tree, while still being very efficient. The balancing is good
 * enough to guarantee <tt>O(log n)</tt> time for most operations.
 *
 * The tree properties are as follows:
 *
 *  1. Each node is colored either red or black.
 *  2. The root is black.
 *  3. All leaves (empty nodes) are black.
 *  4. If a node is red, then both its children are black.
 *  5. Every path from a given node to any of its descendant leaf nodes contains
 *     the same number of black nodes.
 *
 * (excerpted from Wikipedia)
 *
 * For more details see https://en.wikipedia.org/wiki/Red-black_tree
 *
 * \author Brian Kubisiak
 *
 * \copyright This is free and unencumbered software released into the public
 * domain.
 */

#ifndef _RBTREE_H_
#define _RBTREE_H_


#include "utils.h"

/**
 * \brief Node in a red-black tree.
 *
 * Each node in the red-black tree contains its color as well as pointers to its
 * two children: the left (smaller) child and the right (larger) child. The node
 * can be embedded in an existing structure in order to add any additional data
 * needed.
 */
struct rbnode
{
    struct rbnode *left;                        /**< Left child of the node. */
    struct rbnode *right;                       /**< Right child of the node. */
    enum { RB_BLACK, RB_RED } color;            /**< Color of the node. */
};

/**
 * \brief Function for processing nodes in the red-black tree.
 */
typedef int (*RBCallback)(const struct rbnode *node, void *scratch);

/**
 * \brief Red-black self-balancing binary search tree.
 */
struct rbtree
{
    struct rbnode *root; /**< Root node of the tree. */
    cmp_func cmp;        /**< Node comparison function. */
};

void rbtree_init(struct rbtree *tree, cmp_func compare);
struct rbnode *rbtree_search(const struct rbtree *tree,
        const struct rbnode *key);
void rbtree_insert(struct rbtree *tree, struct rbnode *to_add);
int rbtree_traverse(const struct rbtree *tree, RBCallback callback,
        void *scratch);


#endif /* end of include guard: _RBTREE_H_ */

