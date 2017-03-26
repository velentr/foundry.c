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
 * \file rbtree.c
 *
 * \brief Red-black self-balancing binary search tree.
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

#include <assert.h>
#include <stdlib.h>

#include "rbtree.h"
#include "utils.h"

#define RBCHECK(tree) do { \
    assert(_check_node_colors(tree->root) == 0); \
    assert(_is_black(tree->root));               \
    assert(_check_red_nodes(tree->root) == 0);   \
    assert(_get_black_depth(tree->root) >= 0);   \
} while (0)

#define MAX_RBTREE_DEPTH 128

static int _check_node_colors(const struct rbnode *node)
{
    if (node == NULL)
        return 0;
    else if (node->color != RB_BLACK && node->color != RB_RED)
        return -1;
    else if (_check_node_colors(node->left) == -1
          || _check_node_colors(node->right) == -1)
        return -1;
    else
        return 0;
}

static int _is_black(const struct rbnode *node)
{
    return node == NULL || node->color == RB_BLACK;
}

static int _check_red_nodes(const struct rbnode *node)
{
    if (node != NULL && node->color == RB_RED
            && (!_is_black(node->left) || !_is_black(node->right)))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

static int _get_black_depth(const struct rbnode *node)
{
    int left, right;

    if (node == NULL)
        return 1;

    left = _get_black_depth(node->left);
    right = _get_black_depth(node->right);

    if (left == -1 || left != right)
        return -1;

    return left + (_is_black(node) ? 1 : 0);
}

static void _rotate_left(struct rbnode **n)
{
    struct rbnode *parent;

    assert((*n)->right != NULL);

    parent = *n;
    *n = parent->right;
    parent->right = (*n)->left;
    (*n)->left = parent;
}

static void _rotate_right(struct rbnode **n)
{
    struct rbnode *parent;

    assert((*n)->left != NULL);

    parent = *n;
    *n = parent->left;
    parent->left = (*n)->right;
    (*n)->right = parent;
}

static int _traverse(const struct rbnode *node, RBCallback cb, void *scratch)
{
    int rc;

    if (node == NULL)
        return 0;

    rc = _traverse(node->left, cb, scratch);
    if (rc != 0)
        return rc;

    rc = cb(node, scratch);
    if (rc != 0)
        return rc;

    rc = _traverse(node->right, cb, scratch);
    if (rc != 0)
        return rc;

    return 0;
}

/**
 * \brief Initialize the red-black tree.
 *
 * Set up the given red-black tree so it is ready to be used.
 *
 * \param [out] tree Red-black tree to initialize.
 * \param [in] compare Function for comparing two nodes.
 */
void rbtree_init(struct rbtree *tree, cmp_func compare)
{
    assert(tree != NULL);
    assert(compare != NULL);

    tree->root = NULL;
    tree->cmp = compare;
}

/**
 * \brief Search the tree for the node matching \p key.
 *
 * Traverses the red-black tree starting at the root, comparing to the \p key on
 * each iteration. If the \p key is less than the current node, continue
 * searching through the left subtree. If the \p key is greater than the current
 * node, continue searching through the right subtree. This will continue until
 * a node matching the \p key is found, or a leaf is reached. If a leaf is
 * reached, then there was no node in the tree matching the \p key.
 *
 * This function runs in <tt>O(log n)</tt> time with respect to the number of
 * nodes in the tree.
 *
 * \param [in] tree Red-black tree to search for the given \p key.
 * \param [in] key Key to search for in the tree.
 *
 * \return Returns the node matching the given key, or \c NULL if no such node
 * exists in the tree.
 */
struct rbnode *rbtree_search(const struct rbtree *tree,
        const struct rbnode *key)
{
    struct rbnode *cur;
    int cmp;

    assert(tree != NULL);
    assert(key != NULL);

    /* Start searching at the root. */
    cur = tree->root;

    while (cur != NULL)
    {
        cmp = tree->cmp(key, cur);

        /* If key < cur, search the left child. */
        if (cmp < 0)
            cur = cur->left;
        /* If key > cur, search the right child. */
        else if (cmp > 0)
            cur = cur->right;
        /* Else, we found the key. */
        else
            break;
    }

    return cur;
}

void rbtree_insert(struct rbtree *tree, struct rbnode *node)
{
    struct rbnode **backtrace[MAX_RBTREE_DEPTH];
    struct rbnode *uncles[MAX_RBTREE_DEPTH];
    struct rbnode *gparent, *parent, *uncle, *sibling;
    size_t current;
    int backtrack;

    RBCHECK(tree);

    /* Initialize the properties of the new node. */
    node->color = RB_RED;
    node->left = node->right = NULL;

    current = 0;
    backtrace[current] = &tree->root;
    gparent = parent = uncle = sibling = NULL;

    /* Insert the new node, keeping track of the path back to the root. */
    while (*backtrace[current] != NULL)
    {
        gparent = parent;
        parent = *backtrace[current];
        uncle = sibling;

        current++;
        assert(current < MAX_RBTREE_DEPTH);

        uncles[current] = uncle;

        if (tree->cmp(node, parent) < 0)
        {
            backtrace[current] = &parent->left;
            sibling = parent->right;
        }
        else
        {
            backtrace[current] = &parent->right;
            sibling = parent->left;
        }
    }
    *backtrace[current] = node;

    /*
     * The new tree may violate one of the RB-tree invariants; fix any issues
     * that may occur.
     */

    do
    {
        node = *backtrace[current];

        /* Node added at the root; since the root must be black (property 1),
         * just recolor this black, and no other properties are violated.
         */
        if (tree->root == node)
        {
            node->color = RB_BLACK;
            return;
        }

        /* This is not the root node; we must have a parent. */
        assert(current >= 1);
        parent = *backtrace[current-1];
        assert(parent != NULL);

        /* If the parent is black, then no properties are violated; don't have
         * to do anything special to fix the tree.
         */
        if (parent->color == RB_BLACK)
        {
            return;
        }

        /* Since the parent is now red, then the grandparent must exist (i.e.
         * the parent is not the root).
         */
        assert(parent->color == RB_RED);
        assert(current >= 2);
        gparent = *backtrace[current-2];
        assert(gparent != NULL);
        uncle = uncles[current];

        /* If the uncle and the parent are red, then they can be repainted
         * black, and the grandparent (which must be black) can be repainted
         * red.
         */
        if (!_is_black(uncle))
        {
            parent->color  = RB_BLACK;
            uncle->color   = RB_BLACK;
            gparent->color = RB_RED;

            backtrack = 1;
            current -= 2;
        }
        else
        {
            backtrack = 0;
        }
    } while (backtrack);

    /* If the current node is an inner child, rotate it to the outside. */
    if (node == parent->right && parent == gparent->left)
    {
        _rotate_left(backtrace[current-1]);
        parent = node;
    }
    else if (node == parent->left && parent == gparent->right)
    {
        _rotate_right(backtrace[current-1]);
        parent = node;
    }

    /*
     * Note: at this point, the backtrace is only valid up to (including)
     * current-2. It should be trivial to correct, but this is not necessary for
     * the remaining operations.
     */

    /* Now the current node must be an outer child; correct the colors and
     * rotate the grandparent.
     */
    parent->color  = RB_BLACK;
    gparent->color = RB_RED;
    if (parent == gparent->right)
    {
        _rotate_left(backtrace[current-2]);
    }
    else
    {
        assert(parent == gparent->left);
        _rotate_right(backtrace[current-2]);
    }

    RBCHECK(tree);
}

void rbtree_delete(struct rbtree *tree, struct rbnode *to_del)
{
    (void)tree;
    (void)to_del;
}

int rbtree_traverse(struct rbtree *tree, RBCallback callback, void *scratch)
{
    return _traverse(tree->root, callback, scratch);
}

