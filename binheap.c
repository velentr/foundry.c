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
 * \file binheap.c
 *
 * \brief Arbitrarily large binary heaps.
 *
 * This file contains the implementation for a binary heap. The interface is
 * sufficient for creating a priority queue with the heap. In many cases, this
 * implementation optimizes for speed rather than checking input; garbage in,
 * garbage out.
 *
 * These operations will gurentee the heap property on the binary heap. That is,
 * the heap is a complete binary tree, possibly excluding the last level. In
 * addition, each node is less than or equal to both of its children, according
 * to the given comparison function; this makes the heap into a min heap. If a
 * max heap is desired, negate the value returned by the compare function.
 *
 * \author Brian Kubisiak
 *
 * \copyright This is free and unencumbered software released into the public
 * domain.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "binheap.h"

/**
 * \brief Gets the index of the node that is the parent of the node at the given
 * index.
 *
 * This assumes that the nodes of the binary tree are stored in an array. This
 * macro can be used to quickly find the parent of a given node in the array.
 * The argument 'i' must be strictly positive for this macro to work.
 *
 * This operation assumes integer division. Specifically, the
 * parent node is at the index 'floor((i-1) / 2)'. Integer division
 * will automatically do the floor operation.
 *
 * \param [in] i Index of the node to get the parent of.
 *
 * \return Return the index of the parent of the given node.
 */
#define _parent(i)  (((i) - 1) / 2)

/**
 * \brief Gets the index of the left child of the node at the given index.
 *
 * This assumes that the elements of the tree are stored in an array. The
 * argument \p i should be non-negative.
 *
 * Note that both children of a given node are located
 * sequentially. The left child is the one that is at the lower
 * index of the two.
 *
 * \param [in] i Index of the node to get the left child of.
 *
 * \return Returns the index of the left child of the given node.
 */
#define _lchild(i)  (2*(i) + 1)

/**
 * \brief Gets the index of the right child of the node at the given index.
 *
 * This assumes that the elements of the tree are stored in an array. The
 * argument \p i should be non-negative.
 *
 * Note that both children of a given node are located
 * sequentially. The right child is the one that is at the higher
 * index of the two.
 *
 * \param [in] i Index of the node to get the right child of.
 *
 * \return Returns the index of the right child of the given node.
 */
#define _rchild(i)  (2*(i) + 2)

/**
 * \brief Sift a node up through the heap.
 *
 * This function takes a node in the heap where the heap rooted at this node
 * satisifes the heap property, but the parent node may not. This function will
 * then sift the node upwards if needed until the heap property is restored.
 *
 * Assumes that \p i is a valid index in the heap. The worst-case time
 * complexity of this function is <tt>O(log n)</tt>, where \p n is the number of
 * elements on the heap.
 *
 * Even though this function is recursive, GCC will use tail call
 * optimization to convert this to an iterative function.
 *
 * \param [in,out] bh Pointer to the binary heap to sift through.
 * \param [in] i Index of the node to sift upwards, if needed.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre <tt>i < vec_len(&(bh->vec))</tt>
 */
static void _siftup(struct binheap *bh, unsigned int i)
{
    /* The code below will look cleaner if we keep a pointer to the vector in a
     * local variable */
    struct vector *v;

    /* Index of the parent of the current node. */
    unsigned int p;

    assert(bh != NULL);
    assert(i < vec_len(&(bh->vec)));

    v = &(bh->vec);
    p = _parent(i);

    /*
     * Check to make sure the current node is not at the root (otherwise, the
     * parent node does not exist). Then, if the current node is smaller than
     * its parent, we need to swap them.
     */
    if (i > 0 && bh->cmp(vec_get(v, i), vec_get(v, p)) < 0)
    {
        /* Swap the current node with its parent. */
        vec_swap(v, i, p);

        /* Continue to sift the same node up the heap until the heap property is
         * restored. */
        _siftup(bh, p);
    }
}

/**
 * \brief Sift a node down through the heap.
 *
 * This function takes in a node in a heap, where the node may violate the heap
 * property, then sifts its value down the heap in order to satisfy the heap
 * property. Note that both children of this root must satisfy the heap
 * property.
 *
 * This function assumes that the root node in the heap may violate
 * the heap property (i.e. it is greater than one of its children),
 * but heap rooted at both of its children still satisfy this
 * property. Assumes that 'i' is a valid index.
 *
 * The worst-case time complexity of this function is <tt>O(log n)</tt>,
 * where \p n is the number of elements on the heap.
 *
 * Even though this function is recursive, GCC will use tail call
 * optimization to convert this to an iterative function.
 *
 * \param [in,out] bh Pointer to the heap to sift through
 * \param [in] i Index of the node to use a the root of an invalid binary heap.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre <tt>i < vec_len(&(bh->vec))</tt>
 */
static void _siftdown(struct binheap *bh, unsigned int i)
{
    /* The code below will look cleaner if we keep a pointer to the vector in a
     * local variable */
    struct vector *v;

    /* Indices of the left and right children of the root node at i. */
    unsigned int l;
    unsigned int r;

    /*
     * Index of the node that is the smallest of the root and its children.
     * Initialize it to the root, so that it only needs to be changed if one of
     * the children is smaller.
     */
    unsigned int smallest = i;

    assert(bh != NULL);
    assert(vec_isempty(&bh->vec) || i < vec_len(&(bh->vec)));

    v = &(bh->vec);
    l = _lchild(i);
    r = _rchild(i);

    /* Nothing to do if the vector is empty. */
    if (vec_isempty(v))
        return;

    /* Check if the left child exists and the root node is greater than its
     * left child. */
    if (l < vec_len(v) &&
            bh->cmp(vec_get(v, l), vec_get(v, i)) < 0)
    {
        /* The left child is smaller than the parent. Store it here. */
        smallest = l;
    }

    /* Next, check if the right child exists and the smallest node is
     * greater than the right child. */
    if (r < vec_len(v) &&
            bh->cmp(vec_get(v, r), vec_get(v, smallest)) < 0)
    {
        /* If so, right child is the smallest of the three. Store this new
         * index here. */
        smallest = r;
    }

    /*
     * If the root node is not the smallest, then swap it with the smallest
     * node. Now we have a new heap that needs to be sifted, with the root
     * node at index i.
     */
    if (smallest != i)
    {
        /* Swap the root down the heap. */
        vec_swap(v, i, smallest);

        /* Then sift it again until the heap property is restored. */
        _siftdown(bh, smallest);
    }
}

/**
 * \brief Check the heap invariant.
 *
 * This function checks to see if the heap property is violated in the given
 * heap. That is, it checks to see if every parent node is less than or equal to
 * both of its children. It returns a boolean value; returns true if the heap
 * property holds, and returns false if it is violated.
 *
 * This function is used purely for debugging, as the heap property will always
 * hold outside of the functions in this file. It is used for checking
 * postconditions in the functions that manipulate the heap order.
 *
 * \param [in] bh Pointer to the heap to check.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre <tt>i < vec_len(&(bh->vec))</tt>
 */
static int _checkheap(const struct binheap *bh)
{
    unsigned int i;             /* Loop index for iterating over vector. */
    int rc = 0;                 /* Return value for tracking heap violations. */
    const struct vector *v;     /* Stores the vector to iterate over. */

    assert(bh != NULL);

    v = &(bh->vec);

    /* Iterate over every element of the binheap (up to the parent of the last
     * element), ensuring that every element is less than or equal to its
     * children. */
    for (i = 0; i < vec_len(v); i++)
    {
        if (_lchild(i) < vec_len(v)
                && bh->cmp(vec_get(v, i), vec_get(v, _lchild(i))) > 0)
        {
            rc++;
        }
        if (_rchild(i) < vec_len(v)
                && bh->cmp(vec_get(v, i), vec_get(v, _rchild(i))) > 0)
        {
            rc++;
        }
    }

    /* rc contains a count of the violations; want to see if the number of
     * violations is 0. */
    return rc == 0;
}

/**
 * \brief Initialize the given binary heap so that it is ready to be used.
 *
 * This includes initializing the vector, as well as storing the comparison
 * function for sorting the heap. The argument \p elemsize is the size of each
 * element in the binheap. The argument \p size is the initial value for the
 * number of bytes for the initial size of the vector, or \c 0 if the default
 * size is desired. Returns a flag indicating whether or not the operation
 * succeeded (\c 0 for success, \c -1 for failure).
 *
 * \param bh Pointer to the binary heap to initialize.
 * \param cmp Function for comparing two elements of the heap to determine which
 *        is smaller.
 * \param elemsize Size of each element on the heap.
 * \param size Initial value for the number of bytes in the vector. If this
 *             argument is \c 0, then the default size is used.
 * \param alloc Memory allocator used for preforming all allocation for the
 *              binary heap. Interface should be equivalent to \c realloc.
 *              Should be set to \c NULL to use the default allocator.
 *
 * \return If memory is successfully allocated for the vector, returns \c 0. If
 * the memory allocation fails, returns \c -1.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre <tt>cmp != NULL</tt>
 *
 * \note The \p size argument should generally be \c 0, unless there is a
 * specific reason to limit the size of the vector. Note that this argument is
 * given in bytes, not number of elements.
 */
int binheap_init(struct binheap *bh, HeapCompare cmp, size_t elemsize,
        size_t size, void *(*alloc)(void *, size_t))
{
    assert(bh != NULL);
    assert(cmp != NULL);

    /* Store the comparison function in the binheap struct. */
    bh->cmp = cmp;

    /* Initialize the vector. Note that the size argument can be passed
     * directly, and the return value can be returned from here. */
    return vec_init(&(bh->vec), elemsize, size, alloc);
}

/**
 * \brief Push a new element onto the heap.
 *
 * The new element will be added to the bottom, then sifted upwards until the
 * heap property is restored. Since this is a minheap, the new element will be
 * sifted into a place where both children are greater than it. If there is not
 * enough memory for the element to fit on the heap, more memory will
 * automatically be allocated. Returns a code indicating whether or not the
 * element was successfully added to the heap: \c 0 if successful, \c -1 if the
 * heap was not big enough and the memory allocation failed.
 *
 * \param bh Pointer to the heap to add an element to.
 * \param e Element to add to the heap.
 *
 * \return If the element is successfully added to the binary heap, returns \c
 * 0. If there is not enough space on the heap, and the memory allocation fails,
 * returns \c -1.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre Heap property holds.
 *
 * \post Heap property holds.
 *
 * \note This operation has a worst-case time complexity of O(log n) with
 * respect to the size of the heap.
 */
int binheap_push(struct binheap *bh, void *e)
{
    int rc;     /* Variable for holding the return code from pushing the element
                   onto the vector. */

    assert(bh != NULL);
    assert(_checkheap(bh));

    /* Add the new element to the end of the vector. This is the location of the
     * first unfilled node in the lowest level of the tree. */
    rc = vec_push(&(bh->vec), e);

    /* Check if the new element was added successfully. */
    if (rc != -1)
    {
        /* Sift the element upwards until the heap property is restored. */
        _siftup(bh, vec_len(&(bh->vec)) - 1);
        assert(_checkheap(bh));
    }

    return rc;  /* Return the value indicating if the element was successfully
                   added to the vector. */
}

/**
 * \brief Remove the minimum element from the heap.
 *
 * After the element is removed, the element at the tail of the heap is moved to
 * the top, then sifted down in order to restore the heap property. Since this
 * is a minheap, the element will be sifted into a place where both children are
 * greater than it. If there are no elements on the heap, returns -1. In order
 * to get the minimum element without removing it, use the #binheap_peek()
 * function.
 *
 * \param bh Pointer to the heap to pop an element off of.
 *
 * \return Returns 0 if the operation succeeds (i.e. the heap is nonempty).
 * Returns -1 if the operation fails.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre Heap property holds.
 *
 * \post Heap property holds.
 *
 * \note This operation has a worst-case time complexity of O(log n), with
 * respect to the size of the heap.
 */
int binheap_pop(struct binheap *bh)
{
    /* Get a pointer to the vector to simplify the code. */
    struct vector *v;
    int rc;

    assert(bh != NULL);
    assert(_checkheap(bh));

    v = &bh->vec;

    /* If the heap is empty, we have nothing to do. */
    if (vec_isempty(v))
    {
        rc = -1;
    }
    /* Otherwise, swap the first and last elements, pop the last (this is the
     * minimal element on the heap), then sift the first element down.
     */
    else
    {
        vec_swap(v, 0, vec_len(v) - 1);
        vec_pop(v);
        _siftdown(bh, 0);

        rc = 0;
    }

    assert(_checkheap(bh));

    return rc;
}

