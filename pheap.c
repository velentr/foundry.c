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
 * A pairing heap is a heap data structure built using pointers that has
 * relatively low asymptotic and practical performance. All operations have
 * amortized asymptotic time equivalent to Fibonacci heaps, but with a much
 * simpler implementation and lower constants.
 *
 * This implementation supports a min-heap without using any dynamic memory
 * allocation (all memory is allocated by the caller), embedding pointers within
 * container structures in order to simplify memory usage.
 *
 * Pairing heaps were invented by Fredman, Sedgewick, Sleator, and Tarjan,
 * inspired by splay trees. For more information, see
 * https://en.wikipedia.org/wiki/Pairing_heap
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

/**
 * \brief Merge two nodes of the pairing heap.
 *
 * Takes two nodes in a pairing heap and merges them together, maintaining the
 * heap property. To do this, the minimum of the two nodes is set as the root,
 * and the other is merged into its children. When this operation is done, the
 * returned node is the only one of the parameters that should be manipulated
 * further.
 *
 * \param [in] ph The heap containing \p pe0 and \p pe1.
 * \param [in] pe0 The first pairing element to merge.
 * \param [in] pe1 The second pairing element to merge.
 *
 * \return Returns the smaller of the two elements, with the other merged into
 * its children.
 *
 * \pre <tt>ph != NULL</tt>
 * \pre <tt>pe0 != NULL</tt>
 * \pre <tt>pe1 != NULL</tt>
 *
 * \note This operation has time complexity of O(1).
 */
static struct pheap_elem *_merge(const struct pheap *ph, struct pheap_elem *pe0,
        struct pheap_elem *pe1)
{
    struct pheap_elem *ret;

    assert(ph != NULL);
    assert(pe0 != NULL);
    assert(pe1 != NULL);

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

/**
 * \brief Merge pairs of nodes and reverse the order.
 *
 * Given a list of \p src nodes, merge adjacent pairs and reverse the ordering
 * of the result. The resulting merged nodes are stored in \p dst. This is the
 * first merge pass when deleting the minimum node from the heap.
 *
 * \param [in] ph Pairing heap containing the nodes to merge.
 * \param [out] dst List into which the nodes are merged.
 * \param [in] src List from which to merge nodes.
 *
 * \post \c list_isempty(src)
 *
 * \note This operation has a time complexity of O(n) with respect to the length
 * of \p src.
 */
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

/**
 * \brief Merge a list of nodes into a single node.
 *
 * Given a list of \p src nodes, merge them all together to extract the minimum
 * node in the list. This function is the second merge pass when deleting the
 * minimum node in the tree; the result of this function will be the new root of
 * the heap.
 *
 * \param [in] ph Heap containing the list of nodes.
 * \param [in] src List of nodes to merge together.
 *
 * \return Returns the new root of the list.
 *
 * \post \c list_isempty(src)
 *
 * \note This function has a time complexity of O(n) with respect to the length
 * of the list \p src.
 */
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

/**
 * \brief Initialize an element to use with a pairing heap.
 *
 * Since individual elements contain lists, they must be initialized before they
 * are added to a heap. This function will make sure that the element is ready
 * to be used.
 *
 * \param [inout] pe The element to initialize.
 */
void pheap_elem_init(struct pheap_elem *pe)
{
    list_init(&pe->children);
}

/**
 * \brief Initialize a pairing heap for use.
 *
 * This function prepares the heap for use. After calling this function,
 *relements can be added to the heap. Note that the elements themselves must
 * also be initialized before they can be added.
 *
 * \param [inout] ph The heap to initialize.
 * \param [in] cmp Function for comparing two elements on the heap. This
 * function takes as arguments two \c pheap_elem structures, and returns an
 * integer less than, equal to, or greater than zero if the first element is
 * respectively less than, equal to, or greater than the second element.
 */
void pheap_init(struct pheap *ph, cmp_func cmp)
{
    ph->cmp = cmp;
    ph->root = NULL;
}

/**
 * \brief Get the minimum element from the heap without removing it.
 *
 * If there are no elements on the heap, returns \c NULL. To remove the minimum
 * element, use the #pheap_pop() function.
 *
 * \param [in] ph Pairing heap from which to find the minimum element.
 *
 * \return Returns the minimum element on the heap.
 *
 * \note This operation has a time complexity of O(1).
 */
struct pheap_elem *pheap_peek(const struct pheap *ph)
{
    return ph->root;
}

/**
 * \brief Push a new element onto the pairing heap.
 *
 * If the pairing heap is empty, the new element is added as the root.
 * Otherwise, the new element is merged with the root. This operation is thus
 * constant-time, putting off most of the work into the #pheap_pop() function.
 *
 * \param [in] ph Heap onto which the new element is pushed.
 * \param [in] pe New element to push onto the heap.
 *
 * \note This operation has a time-complexity of O(1).
 */
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

/**
 * \brief Remove the minimum element from the pairing heap.
 *
 * After the element is removed, its children are merged in pairs, then the
 * resulting list of children is merged into a new root.
 *
 * This operation will run in time O(n) with respect to the number of children
 * of the root. As a result, it will be amortized to O(log n) with respect to
 * the size of the heap. See https://en.wikipedia.org/wiki/Pairing_heap for
 * further analysis of the time complexity.
 *
 * \param [in] ph Pairing heap from which to remove the smallest element.
 *
 * \note This operation has an amortized time-complexity O(log n) with respect
 * to the number of elements on the heap.
 */
struct pheap_elem *pheap_pop(struct pheap *ph)
{
    struct list merged;
    struct pheap_elem *ret;

    ret = ph->root;

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

    return ret;
}

/**
 * \brief Merge together two heaps.
 *
 * Merges all the elements from \p src into the heap \p dst. After this
 * operation, \p src will be empty and \p dst will contain all elements that
 * were previously in either \p src and \p dst.
 *
 * \param [out] dst Heap into which the elements of \p src are merged.
 * \param [in] src Heap to merge into \p dst.
 *
 * \note This operation has a time-complexity of O(1).
 */
void pheap_merge(struct pheap *dst, struct pheap *src)
{
    assert(dst->cmp == src->cmp);

    if (!pheap_isempty(src))
    {
        pheap_push(dst, src->root);
        src->root = NULL;
    }

    assert(pheap_isempty(src));
}

/**
 * \brief Check if the given heap is empty.
 *
 * \param [in] ph Pairing heap to check for emptiness.
 *
 * \return Returns 0 is there are elements on the heap. Returns nonzero if there
 * are no elements on the heap.
 *
 * \note This operation has a time-complexity of O(1).
 */
int pheap_isempty(struct pheap *ph)
{
    return ph->root == NULL;
}

