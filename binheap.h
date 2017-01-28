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
 * \file binheap.h
 *
 * \brief Arbitrarily large binary heaps.
 *
 * Implements a binary heap. The interface is sufficient for creating a priority
 * queue with the heap. In many cases, this implementation optimizes for speed
 * rather than checking input; garbage in, garbage out.
 *
 * These operations will guarantee the heap property on the binary heap. That
 * is, the heap is a complete binary tree, possibly excluding the last level. In
 * addition, each node is less than or equal to both of its children, according
 * to the given comparison function; this makes the heap into a min heap. If a
 * max heap is desired, negate the value returned by the compare function.
 */

#ifndef _BINHEAP_H_
#define _BINHEAP_H_

/**
 * Need to use the vector for internal storage on the binary heap.
 */
#include "vector.h"

/**
 * Used as the default size for a new binary heap, if a size is not specified in
 * the init function. Right now, just uses the default vector size.
 */
#define DEF_BHEAP_LEN  DEF_VEC_LEN


/**
 * \brief Pointer to a function that is used for comparing two items on a heap.
 *
 * The function should compare the two elements and return a number less than,
 * equal to, or greater than zero if the first element is respectively less
 * than, equal to, or greater than the second element. The magnitude of the
 * return value does not matter, just the sign.
 *
 * \param a The first element to compare.
 * \param b The second element to compare.
 *
 * \return Returns an integer that is used to determine which element is
 * smaller, for sorting in the heap. Returns less than, equal to, or greater
 * than zero if the first argument is respectively less than, equal to, or
 * greater than the second argument.
 *
 * \note Generally, a function pointer like this should have a scratch argument
 * so global variables are not needed. However, in a heap, there should not be
 * any global state that affects the sorting order, so a scratch argument
 * shouldn't be necessary in this case.
 */
typedef int (*HeapCompare)(const void *a, const void *b);

/**
 * \brief Memory for storing a binary heap.
 *
 * In this heap, the minimum element is sifted to the top of the heap; that is,
 * this is a minheap. This structure is usually used for creating a priority
 * queue, where the highest priority is indicated by the element with the
 * minimum value. If a maxheap is needed, just negate the output of the compare
 * function. Since a vector is used for storing the data, the binary heap will
 * grow arbitrarily large to fit all the elements.
 *
 * When using a binary heap, first call the #bheap_init() function to initialize
 * the vector and set the comparison function. Then, any of the \c bheap_*
 * functions can be called to manipulate the binary heap.
 *
 * The vector of the heap should never be accessed directly, and the compare
 * function should not be changed after filling the heap. Most of the functions
 * assume that the heap does not violate certain properties; modifying the
 * vector or the compare function will cause these heap properties to be
 * violated.
 *
 * When the binary heap is no longer needed, call the #bheap_free() function to
 * free the memory from the vector. If the elements of the heap are dymanically
 * allocated, they should all be popped and freed before calling the
 * #bheap_free() function. Otherwise, the references inside the heap will be
 * lost, and memory may be leaked. Once #bheap_free() is called on the heap, it
 * should no longer be used unless #bheap_init() is called on it again.
 */
struct binheap
{
    struct vector vec; /**< Vector holding all the elements in the heap. */
    HeapCompare cmp;   /**< Comparison function used for sorting the elements on
                            the heap. See the description of the HeapCompare
                            typedef for more information. */
};


int bheap_init(struct binheap *bh, HeapCompare cmp, size_t elemsize,
        size_t size, void *(*alloc)(void *, size_t));
void bheap_free(struct binheap *bh);
unsigned int bheap_len(const struct binheap *bh);
unsigned int bheap_space(const struct binheap *bh);
int bheap_isempty(const struct binheap *bh);
int bheap_push(struct binheap *bh, void *e);
int bheap_pop(struct binheap *bh);
void *bheap_peek(const struct binheap *bh);


#endif /* end of include guard: _BINHEAP_H_ */
