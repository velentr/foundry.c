/*
 * MIT License (MIT)
 *
 * Copyright (c) 2014 Brian Kubisiak <velentr.rc@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
 * Notes:       Assumes that 'i' is a valid index in the heap. The worst-case
 *              time complexity of this function is 'O(log n)', where 'n' is the
 *              number of elements on the heap.
 */
void _siftup(struct binheap *bh, unsigned int i)
{
    /* The code below will look cleaner if we keep a pointer to the vector in a
     * local variable */
    struct vector *v = &(bh->vec);

    /* Index of the parent of the current node. */
    unsigned int p = _parent(i);

    /* Keep looping while the current index is valid and the current node is
     * less than its parent. */
    while (i > 0 && bh->cmp(vec_get(v, i), vec_get(v, p)) < 0)
    {
        /*
         * The current node is smaller than its parent. Swap it with the parent
         * to sift it upwards in the heap. Then, we need to continue to sift it
         * upwards.
         */
        vec_swap(v, i, p);


        i = p;                  /* The current node is now at the index p. */
        p = _parent(i);         /* Get the new parent of the current node. */
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
 * Notes:       This function assumes that the root node in the heap may violate
 *              the heap property (i.e. it is greater than one of its children),
 *              but heap rooted at both of its children still satisfy this
 *              property. Assumes that 'i' is a valid index.
 *
 *              The worst-case time complexity of this function is 'O(log n)',
 *              where 'n' is the number of elements on the heap.
 */
void _siftdown(struct binheap *bh, unsigned int i)
{
    /* The code below will look cleaner if we keep a pointer to the vector in a
     * local variable */
    struct vector *v = &(bh->vec);

    /* Indices of the left and right children of the root node at i. */
    unsigned int l, r;

    /*
     * Index of the node that is the smallest of the root and its children.
     * Initialize it to the root, so that it only needs to be changed if one of
     * the children is smaller.
     */
    unsigned int smallest = i;

    /* We want to loop this as long as the root node is not the smallest. */
    do {
        /*
         * Here we set the root node to be the smallest node of the previous
         * iteration. Note that this is redundant for the first iteration. We
         * must also get its child indices here.
         */
        i = smallest;
        l = _lchild(i);
        r = _rchild(i);

        /*
         * Check if the left child exists and the root node is greater than its
         * left child. Note that the root node always exists.
         */
        if (l < vec_size(v) &&
                bh->cmp(vec_get(v, l), vec_get(v, i)) < 0)
        {
            /* The left child is smaller than the parent. Store it here. */
            smallest = l;
        }
        /*
         * Next, check if the right child exists and the smallest node is
         * greater than the right child.
         */
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
            vec_swap(v, i, smallest);
        }

        /*
         * Continue to loop until the root node is the smallest. At this point,
         * the heap property is restored.
         */
    } while (smallest != i);
}

/*
 * Initialize the binary heap so that it is empty and ready to use. Store the
 * comparison function in the structure and initialize the vector.
 */
int bheap_init(struct binheap *bh, HeapCompare cmp, size_t size)
{
    /* Store the comparison function in the binheap struct. */
    bh->cmp = cmp;

    /* Initialize the vector. Note that the size argument can be passed
     * directly, and the return value can be returned from here. */
    return vec_init(&(bh->vec), size);
}

/*
 * Deinitialize the heap by freeing the memory associated with the vector. The
 * compare function does not use any dynamic memory, so we don't need to do
 * anything with it.
 */
void bheap_free(struct binheap *bh)
{
    /* Deinitialize the vector holding the heap data. */
    vec_free(&(bh->vec));
}

/*
 * Get the number of elements stored in the vector. The vector stores this
 * number in one of its members, so this is a really fast operation.
 */
unsigned int bheap_size(struct binheap *bh)
{
    return vec_size(&(bh->vec));
}

/*
 * Get the number of spaces in the vector that stores the data for the binary
 * heap. Just acts as a wrapper around the vec_space function.
 */
unsigned int bheap_space(struct binheap *bh)
{
    return vec_space(&(bh->vec));
}

/*
 * Check if the binary heap is empty. The heap is empty if and only if the
 * vector holding its data is empty.
 */
int bheap_isempty(struct binheap *bh)
{
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

    /* Add the new element to the end of the vector. This is the location of the
     * first unfilled node in the lowest level of the tree. */
    rc = vec_push(&(bh->vec), e);

    /* Check if the new element was added successfully. */
    if (rc == 0)
    {
        /* Sift the element upwards until the heap property is restored. */
        _siftup(bh, vec_size(&(bh->vec)) - 1);
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
    struct vector *v = &(bh->vec);

    /* Get the minimum element from the heap, which is currently stored at the
     * head of the vector. Note this might be NULL is the vector is empty. */
    void *min = vec_head(v);

    /* Get the old tail of the vector. May be NULL if the vector is empty. */
    void *tail = vec_pop(v);

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
    }

    /* The element 'min' has now been removed from the heap. */
    return min;
}

/*
 * We can just return the head of the vector. Since the heap property is
 * satisfied, this will be the minimum element of the heap.
 */
void *bheap_peek(struct binheap *bh)
{
    return vec_head(&(bh->vec));
}
