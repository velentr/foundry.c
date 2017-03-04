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
 * \file pheap.c
 *
 * \brief Arbitrarily large pairing heaps.
 *
 * \author Brian Kubisiak
 *
 * \copyright This is free and unencumbered software released into the public
 * domain.
 */

#include <assert.h>

#include "list.h"
#include "pheap.h"
#include "utils.h"

static struct pheap_elem *_merge(const struct pheap *ph, struct pheap_elem *pe0,
        struct pheap_elem *pe1)
{
    struct pheap_elem *ret;

    if (ph->cmp(pe0, pe1) < 0)
    {
        list_pushfront(&pe0->children, &pe1->child_le);
        ret = pe0;
    }
    else
    {
        list_pushfront(&pe1->children, &pe0->child_le);
        ret = pe1;
    }

    return ret;
}

static void _merge_pairs_reverse(const struct pheap *ph, struct list *dst,
        struct list *src)
{
    struct pheap_elem *left, *right;

    /* Move left to right, merging pairs */
    while (!list_isempty(src))
    {
        left = containerof(list_popfront(src),
                struct pheap_elem, child_le);
        if (!list_isempty(src))
        {
            right = containerof(list_popfront(src),
                    struct pheap_elem, child_le);
            left = _merge(ph, left, right);
        }

        /* Push the merged element onto the front of the destination, so that
         * the result is in reverse order.
         */
        list_pushfront(dst, &left->child_le);
    }
}

static struct pheap_elem *_accumulate(const struct pheap *ph, struct list *src)
{
    struct pheap_elem *ret;
    struct pheap_elem *front;

    if (list_isempty(src))
    {
        ret = NULL;
    }
    else
    {
        ret = containerof(list_popfront(src), struct pheap_elem, child_le);
    }

    while (!list_isempty(src))
    {
        front = containerof(list_popfront(src), struct pheap_elem, child_le);
        ret = _merge(ph, ret, front);
    }

    return ret;
}

void pheap_elem_init(struct pheap_elem *pe)
{
    list_init(&pe->children);
}

void pheap_init(struct pheap *ph, cmp_func cmp)
{
    ph->cmp = cmp;
    ph->root = NULL;
}

struct pheap_elem *pheap_peek(const struct pheap *ph)
{
    return ph->root;
}

void pheap_push(struct pheap *ph, struct pheap_elem *pe)
{
    assert(pe != NULL);

    if (pheap_isempty(ph))
    {
        ph->root = pe;
    }
    else
    {
        ph->root = _merge(ph, ph->root, pe);
    }
}

void pheap_pop(struct pheap *ph)
{
    struct list merged;

    /* If the heap is empty, then we have nothing to do. */
    if (!pheap_isempty(ph))
    {
        list_init(&merged);

        /* Reduce the number of nodes to handle by a factor of 2. This is what
         * gives us the amortized O(log n) time for the structure. If the root
         * has any remaining children after this then we leaked a reference to a
         * node.
         */
        _merge_pairs_reverse(ph, &merged, &ph->root->children);
        assert(list_isempty(&ph->root->children));

        /* Now merge all the nodes into a single node, giving us the new root of
         * the heap. If the merged list has any nodes after this operation, then
         * we leaked a reference to a node.
         */
        ph->root = _accumulate(ph, &merged);
        assert(list_isempty(&merged));
    }
}

int pheap_isempty(struct pheap *ph)
{
    return ph->root == NULL;
}

