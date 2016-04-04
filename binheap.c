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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */


/*
 * binheap.c
 *
 * Arbitrarily large binary heaps, public interface.
 *
 * This file contains the public interface for a binary heap. The interface is
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
 * Macros:
 *      _parent         Get the index of the parent of the node at the
 *                      given index.
 *      _lchild         Get the index of the left child node of the node at the
 *                      given index.
 *      _rchild         Get the index of the right child node of the node at the
 *                      given index.
 * Local Functions:
 *      _siftup         Take a node that is greater than its parent and sift it
 *                      upwards until the heap property is restored.
 *      _siftdown       Take an invalid parent of two valid heaps, and sift it
 *                      down until the heap is valid again.
 *      _checkheap      Verify the heap property for the given binheap. Useful
 *                      only for debugging.
 * Functions:
 *      bheap_init      Initialize memory for the binary heap. Must be called
 *                      before the heap can be used.
 *      bheap_free      Free all the memory associated with the heap.
 *      bheap_isempty   Determines if the heap is empty.
 *      bheap_size      Gets the number of elements currently stored in the
 *                      heap.
 *      bheap_space     Returns the number of elements that can currently be
 *                      stored on the heap. The heap will automatically expand
 *                      past this point as needed.
 *      bheap_push      Add a new element onto the heap.
 *      bheap_pop       Remove the minimum element from the heap.
 *      bheap_peek      Get the minimum element without removing it from the
 *                      heap.
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "binheap.h"

/*
 * _parent
 *
 * Description: Gets the index of the node that is the parent of the node at the
 *              given index. This assumes that the nodes of the binary tree are
 *              stored in an array. This macro can be used to quickly find the
 *              parent of a given node in the array. The argument 'i' must be
 *              strictly positive for this macro to work.
 *
 * Arguments:   i   Index of the node to get the parent of.
 *
 * Returns:     Return the index of the parent of the given node.
 *
 * Notes:       This operation assumes integer division. Specifically, the
 *              parent node is at the index 'floor((i-1) / 2)'. Integer division
 *              will automatically do the floor operation.
 */
#define _parent(i)  (((i) - 1) / 2)

/*
 * _lchild
 *
 * Description: Gets the index of the left child of the node at the given index.
 *              This assumes that the elements of the tree are stored in an
 *              array. The argument 'i' should be non-negative.
 *
 * Arguments:   i   Index of the node to get the left child of.
 *
 * Returns:     Returns the index of the left child of the given node.
 *
 * Notes:       Note that both children of a given node are located
 *              sequentially. The left child is the one that is at the lower
 *              index of the two.
 */
#define _lchild(i)  (2*(i) + 1)

/*
 * _rchild
 *
 * Description: Gets the index of the right child of the node at the given
 *              index.  This assumes that the elements of the tree are stored in
 *              an array. The argument 'i' should be non-negative.
 *
 * Arguments:   i   Index of the node to get the right child of.
 *
 * Returns:     Returns the index of the right child of the given node.
 *
 * Notes:       Note that both children of a given node are located
 *              sequentially. The right child is the one that is at the higher
 *              index of the two.
 */
#define _rchild(i)  (2*(i) + 2)

/*
 * _siftup
 *
 * Description: This function takes a node in the heap where the heap rooted at
 *              this node satisifes the heap property, but the parent node may
 *              not. This function will then sift the node upwards if needed
 *              until the heap property is restored.
 *
 * Arguments:   bh  Pointer to the binary heap to sift through.
 *              i   Index of the node to sift upwards, if needed.
 *
 * Pre:         bh != NULL
 *              i < vec_size(&(bh->vec))
 *
 * Notes:       Assumes that 'i' is a valid index in the heap. The worst-case
 *              time complexity of this function is 'O(log n)', where 'n' is the
 *              number of elements on the heap.
 *
 *              Even though this function is recursive, GCC will use tail call
 *              optimization to convert this to an iterative function.
 */
static void _siftup(struct binheap *bh, unsigned int i)
{
    /* The code below will look cleaner if we keep a pointer to the vector in a
     * local variable */
    struct vector *v;

    /* Index of the parent of the current node. */
    unsigned int p;

    assert(bh != NULL);
    assert(i < vec_size(&(bh->vec)));

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

/*
 * _siftdown
 *
 * Description: This function takes in a node in a heap, where the node may
 *              violate the heap property, then sifts its value down the heap in
 *              order to satisfy the heap property. Note that both children of
 *              this root must satisfy the heap property.
 *
 * Arguments:   bh  Pointer to the heap to sift through
 *              i   Index of the node to use a the root of an invalid binary
 *                  heap.
 *
 * Pre:         bh != NULL
 *              i < vec_size(&(bh->vec))
 *
 * Notes:       This function assumes that the root node in the heap may violate
 *              the heap property (i.e. it is greater than one of its children),
 *              but heap rooted at both of its children still satisfy this
 *              property. Assumes that 'i' is a valid index.
 *
 *              The worst-case time complexity of this function is 'O(log n)',
 *              where 'n' is the number of elements on the heap.
 *
 *              Even though this function is recursive, GCC will use tail call
 *              optimization to convert this to an iterative function.
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
    assert(i < vec_size(&(bh->vec)));

    v = &(bh->vec);
    l = _lchild(i);
    r = _rchild(i);

    /* Check if the left child exists and the root node is greater than its
     * left child. */
    if (l < vec_size(v) &&
            bh->cmp(vec_get(v, l), vec_get(v, i)) < 0)
    {
        /* The left child is smaller than the parent. Store it here. */
        smallest = l;
    }

    /* Next, check if the right child exists and the smallest node is
     * greater than the right child. */
    if (r < vec_size(v) &&
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

/*
 * _checkheap
 *
 * Description: This function checks to see if the heap property is violated in
 *              the given heap. That is, it checks to see if every parent node
 *              is less than or equal to both of its children. It returns a
 *              boolean value; returns true if the heap property holds, and
 *              returns false if it is violated.
 *
 * Arguments:   bh  Pointer to the heap to check.
 *
 * Pre:         bh != NULL
 *              i < vec_size(&(bh->vec))
 *
 * Notes:       This function is used purely for debugging, as the heap property
 *              will always hold outside of the functions in this file. It is
 *              used for checking postconditions in the functions that
 *              manipulate the heap order.
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
    for (i = 0; i < vec_size(v); i++)
    {
        if (_lchild(i) < vec_size(v)
                && bh->cmp(vec_get(v, i), vec_get(v, _lchild(i))) > 0)
        {
            rc++;
        }
        if (_rchild(i) < vec_size(v)
                && bh->cmp(vec_get(v, i), vec_get(v, _rchild(i))) > 0)
        {
            rc++;
        }
    }

    /* rc contains a count of the violations; want to see if the number of
     * violations is 0. */
    return rc == 0;
}

/*
 * Initialize the binary heap so that it is empty and ready to use. Store the
 * comparison function in the structure and initialize the vector.
 */
int bheap_init(struct binheap *bh, HeapCompare cmp, size_t size,
        void *(*alloc)(void *, size_t))
{
    assert(bh != NULL);
    assert(cmp != NULL);

    /* Store the comparison function in the binheap struct. */
    bh->cmp = cmp;

    /* Initialize the vector. Note that the size argument can be passed
     * directly, and the return value can be returned from here. */
    return vec_init(&(bh->vec), size, alloc);
}

/*
 * Deinitialize the heap by freeing the memory associated with the vector. The
 * compare function does not use any dynamic memory, so we don't need to do
 * anything with it.
 */
void bheap_free(struct binheap *bh)
{
    assert(bh != NULL);

    /* Deinitialize the vector holding the heap data. */
    vec_free(&(bh->vec));
}

/*
 * Get the number of elements stored in the vector. The vector stores this
 * number in one of its members, so this is a really fast operation.
 */
unsigned int bheap_size(const struct binheap *bh)
{
    assert(bh != NULL);

    return vec_size(&(bh->vec));
}

/*
 * Get the number of spaces in the vector that stores the data for the binary
 * heap. Just acts as a wrapper around the vec_space function.
 */
unsigned int bheap_space(const struct binheap *bh)
{
    assert(bh != NULL);

    return vec_space(&(bh->vec));
}

/*
 * Check if the binary heap is empty. The heap is empty if and only if the
 * vector holding its data is empty.
 */
int bheap_isempty(const struct binheap *bh)
{
    assert(bh != NULL);

    return vec_isempty(&(bh->vec));
}

/*
 * Add a new element to the bottom of the heap, then sift it upwards until the
 * heap property is restored.
 */
int bheap_push(struct binheap *bh, void *e)
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
        _siftup(bh, vec_size(&(bh->vec)) - 1);
        assert(_checkheap(bh));
    }

    return rc;  /* Return the value indicating if the element was successfully
                   added to the vector. */
}

/*
 * Get the element at the head of the vector and save it to return. Pop the last
 * element from the vector, put it in index 0 (replacing the element we just
 * got), then sift this new element down the heap. Return the old head.
 */
void *bheap_pop(struct binheap *bh)
{
    /* Get a pointer to the vector to simplify the code. */
    struct vector *v;

    /* Get the minimum element from the heap, which is currently stored at the
     * head of the vector. Note this might be NULL is the vector is empty. */
    void *min;

    /* Get the old tail of the vector. May be NULL if the vector is empty. */
    void *tail;

    assert(bh != NULL);
    assert(_checkheap(bh));

    v = &(bh->vec);
    min = vec_head(v);
    tail = vec_pop(v);

    /*
     * If the vector is empty, then either there were no elements on the heap to
     * begin with (in which case 'min' will be NULL), or there was only one
     * element on the heap (in which case, 'min' holds this value). In either
     * case, we can just return 'min'. If the heap is not empty, then we must
     * set the new head and sift it.
     */
    if (!vec_isempty(v))
    {
        /*
         * Replace the first element with the old tail in order to remove the
         * old minimum from the heap. Note that we still have a reference to the
         * old first element in 'min'.
         */
        vec_set(v, 0, tail);

        /* The heap property is probably violated; sift the head down. */
        _siftdown(bh, 0);
        assert(_checkheap(bh));
    }

    /* The element 'min' has now been removed from the heap. */
    return min;
}

/*
 * We can just return the head of the vector. Since the heap property is
 * satisfied, this will be the minimum element of the heap.
 */
void *bheap_peek(const struct binheap *bh)
{
    assert(bh != NULL);
    assert(_checkheap(bh));

    return vec_head(&(bh->vec));
}

