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



/**
 * \file binheap.h
 *
 * \brief Arbitrarily large binary heaps.
 *
 * Implements a binary heap. The interface is sufficient for creating a priority
 * queue with the heap. In many cases, this implementation optimizes for speed
 * rather than checking input; garbage in, garbage out.
 *
 * These operations will gurentee the heap property on the binary heap. That is,
 * the heap is a complete binary tree, possibly excluding the last level. In
 * addition, each node is less than or equal to both of its children, according
 * to the given comparison function; this makes the heap into a min heap. If a
 * max heap is desired, negate the value returned by the compare function.
 *
 * Datatypes:
 *      struct binheap  Heap datastructure implemented using a complete binary
 *                      tree where both child nodes are less than the parent.
 *                      The implementation is thus a minheap. Uses a vector for
 *                      storing data. Memory usage is very efficient.
 *
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
#define DEF_BHEAP_SIZE  DEF_VEC_SIZE

/**
 * Defines the size of each element in the binary heap. Since a vector is used
 * for the storage, then this is just the element size for the vector.
 */
#define BHEAP_ELEMSIZE  VEC_ELEMSIZE



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
    HeapCompare cmp;   /**< Comparison function used for sorting the elements on
                            the heap. See the description of the HeapCompare
                            typedef for more information. */
    struct vector vec; /**< Vector holding all the elements in the heap. */
};


/**
 * \brief Initialize the given binary heap so that it is ready to be used.
 *
 * This includes initializing the vector, as well as storing the comparison
 * function for sorting the heap. The argument \p size is the initial value for
 * the number of bytes for the initial size of the vector, or \c 0 if the
 * default size is desired. Returns a flag indicating whether or not the
 * operation succeeded (\c 0 for success, \c -1 for failure).
 *
 * \param bh Pointer to the binary heap to initialize.
 * \param cmp Function for comparing two elements of the heap to determine which
 *        is smaller.
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
 * given in bytes, not number of elements. To get the size for a vector with \c
 * n elements, pass <tt>n * BHEAP_ELEMSIZE</tt>.
 */
int bheap_init(struct binheap *bh, HeapCompare cmp, size_t size,
        void *(*alloc)(void *, size_t));

/**
 * \brief Free all the dynamically-allocated memory associated with the given
 * binary heap.
 *
 * Does not actually change any of the members of the heap, just frees the
 * vector used for storing data.
 *
 * \param bh Pointer to the binary heap to free.
 *
 * \pre <tt>bh != NULL</tt>
 *
 * \note If any of the elements of the heap need to be freed, they should be
 * freed before calling this function. The best way to do this is to pop them
 * all until the heap is empty, freeing them individually.
 */
void bheap_free(struct binheap *bh);

/**
 * \brief Get the total number of elements currently stored on the binary heap.
 *
 * \param bh Pointer to the binary heap to count the elements of.
 *
 * \return Returns the number of elements currently stored on the binary heap.
 *
 * \pre <tt>bh != NULL</tt>
 *
 * \note Note this returns the number of elements on the heap, not the amount of
 * space held by the heap.
 */
unsigned int bheap_size(const struct binheap *bh);

/**
 * \brief Get the total number of elements that can currently be stored in the
 * heap.
 *
 * This is the number of elements that the heap's vector can contain before it
 * must be expanded.
 *
 * \param bh Pointer to the binary heap to count the total spaces of.
 *
 * \return Returns the total number of elements that can be stored in the binary
 * heap before it must be expanded.
 *
 * \pre <tt>bh != NULL</tt>
 *
 * \note This gives the number of elements that can fit, not the number of
 * bytes. It generally does not need to be called, because the binary heap will
 * automatically expand when needed.
 */
unsigned int bheap_space(const struct binheap *bh);

/**
 * \brief Determines if the binary heap is empty.
 *
 * Returns true if the heap is empty; returns false if the heap is not empty.
 *
 * \param bh Pointer to the binary heap to check for emptiness.
 *
 * \return If the number of elements in the vector is \c 0, returns true. Else,
 * returns false.
 *
 * \pre <tt>bh != NULL</tt>
 */
int bheap_isempty(const struct binheap *bh);

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
int bheap_push(struct binheap *bh, void *e);

/**
 * \brief Get the minimum element from the heap and remove that element from the
 * heap.
 *
 * After the element is removed, the element at the tail of the heap is moved to
 * the top, then sifted down in order to restore the heap property. Since this
 * is a minheap, the element will be sifted into a place where both children are
 * greater than it. If there are no elements on the heap, returns \c NULL. In
 * order to get the minimum element without removing it, use the #bheap_peek()
 * function.
 *
 * \param bh Pointer to the heap to pop an element off of.
 *
 * \return Returns the minimum element from the heap, or \c NULL if the heap is
 * empty.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre Heap property holds.
 *
 * \post Heap property holds.
 *
 * \note This operation has a worst-case time complexity of O(log n), with
 * repect to the size of the heap.
 */
void *bheap_pop(struct binheap *bh);

/**
 * \brief Get the minimum element from the heap without removing it from the
 * heap.
 *
 * If there are no elements on the heap, returns \c NULL.
 *
 * \param bh Pointer to the binary heap to find the minimum from.
 *
 * \return Returns the minimum element from the heap, or \c NULL if the heap is
 * empty.
 *
 * \pre <tt>bh != NULL</tt>
 * \pre Heap property holds.
 *
 * \note This operation has a worst-case time complexity of O(1), with respect
 * to the size of the heap.
 */
void *bheap_peek(const struct binheap *bh);


#endif /* end of include guard: _BINHEAP_H_ */
