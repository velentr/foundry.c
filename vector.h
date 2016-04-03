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
 * \file vector.h
 *
 * \author Brian Kubisiak
 *
 * \brief Dynamically-resizable generic arrays.
 *
 * Implementation for dynamically-resizable arrays of generic void pointers. The
 * interface for this datastructure is similar to STL vectors, but without
 * templates or classes. In many cases, optimizes for speed rather than checking
 * inputs; garbage in, garbage out.
 *
 * Vectors using these functions are assumed to be dense. That is, there are no
 * gaps in between the valid elements in the vector. Also note that the order of
 * the elements within the vector is guarenteed to be constant, while the
 * indices are subject to change (e.g. if an element is deleted).
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_


/**
 * Used as the default size for a new vector if a size is not specified in the
 * init function.
 */
#define DEF_VEC_SIZE    32

/**
 * Defines the size of each element in the vector. Useful when converting
 * between number of bytes and number of elements.
 */
#define VEC_ELEMSIZE    sizeof(void *)



/**
 * \brief Array-like data type that can store a set of void pointers.
 *
 * This structure contains all the variables and memory necessary for storing a
 * dynamically-resizable array of void pointers. This is used mainly when the
 * number of elements in the array is not known beforehand, or it is desirable
 * to allow the array to grow with the data. Note that the order of the data
 * stored in the vector will not change based on any of the operators in this
 * file, but the indices may change.
 *
 * size    Stores a count of the total number of elements in the
 *                      vector.
 *              space   Stores the current number of elements that can fit in
 *                      the allocated array. Used for determining when the
 *                      vector must grow.
 *              data    Dynamically allocated array of void pointers for holding
 *                      any data type. Size may change as more elements are
 *                      added.
 *
 * When using a vector, first call the #vec_init() function to initialize the
 * size and space, and to allocate a new buffer for the data. Then, any of the
 * \c vec_* functions may be called to operate on the vector.
 *
 * None of the \c vec_* functions will change the order of the elements in the
 * vector; however, some may change to values of the elements, or change the
 * indices of the elements (i.e. when an element is deleted).
 *
 * When the vector is no longer needed, call the #vec_free() function to free
 * the buffer for this structure. If the elements of the vector are dynamically
 * allocated, it might be necessary to use the #vec_map() function to call a
 * deallocator on each individual element before calling #vec_free(). Once
 * #vec_free() has been called on the vector, then it should no longer be used,
 * unless #vec_init() is called on it again.
 */
struct vector
{
    unsigned int size;  /**< Count of the number of elements in the vector. */
    unsigned int space; /**< Number of elements that can currently fit in the
                             allocated memory. */
    void **data;        /**< Dynamically-allocated array for holding data. */
    void *(*realloc)(void *, size_t); /**< Allocator for getting more memory. */
};


/**
 * \brief Function that operates on each element of a vector, returning the new
 * value for the element.
 *
 * This indicates a pointer to a function that can be mapped over a vector.
 * Takes in an element, the element's index, and a user-supplied argument.
 * Returns a new element to replace the one that was passed as an argument. It
 * can be used to implement a wide variety of filters, iterators, or
 * accumulators. See the #vec_map() function for details of how this is used.
 *
 * \param e Element to operate on.
 * \param i Index of the element being operated on.
 * \param scr Scratch pointer passed to the function unmodified by the
 *        #vec_map() function. This is used to pass other arguments to the
 *        operator.
 *
 * \return Returns a new element to be written back into the vector at the
 *         location of the current element (i.e. at the index i).
 *
 * \note If a value needs to be returned outside the #vec_map() function (e.g.
 *       if you are summing the elements), then use the scratch pointer to hold
 *       the return value.
 */
typedef void *(*VecOperator)(void *e, unsigned int i, void *scr);


/**
 * \brief Initialize the memory for the vector. Must be called before the vector
 * can be used.
 *
 * Initialize the given vector so that it is ready to be used. This includes
 * allocating the memory for the vector, as well as setting initial values for
 * the member variables. The argument \p v is a pointer to the vector to
 * initialize. The argument \p size is an initial value for the number of bytes
 * in the vector, or \c 0 if the default size is desired. Returns a flag
 * indicating whether or not the operation succeeded.
 *
 * \param v Pointer to the vector to initialize.
 * \param size Initial value for the number of bytes in the vector. If this
 *        argument is \c 0, then the default size will be used.  It is important
 *        to note that this is the number bytes, not the number of elements.
 * \param alloc Memory allocator used for performing all allocation for the
 *        vector. Interface should be equivalent to \c realloc. Should be set to
 *        \c NULL to use the default allocator.
 *
 * \return If the memory is successfully allocated, returns \c 0. If the memory
 *         allocation fails, returns \c -1.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note The \p size argument shouldn't generally be used unless there is a
 *       specific reason to limit the size of the vector (or if the needed size
 *       of the vector is known beforehand). It should only be used when extreme
 *       performance is needed; you should probably just pass \c 0.
 *
 * \note Also, it is the size in bytes, not the number of elements. To get the
 *       size for a vector with \c n elements, pass <tt>n * VEC_ELEMSIZE</tt>.
 */
int vec_init(struct vector *v, size_t size, void *(*alloc)(void *, size_t));

/**
 * \brief Free all the memory associated with the vector.
 *
 * Free the memory used by the given vector. This will change some members of
 * the vector, but the vector should not be used after being freed, so this
 * shouldn't matter.
 *
 * \param v Pointer to the vector to free.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note If any of the elements in the buffer need to be freed, they should be
 * freed before calling this function. It is probably easiest to just call the
 * #vec_map() function with the element deallocator.
 */
void vec_free(struct vector *v);

/**
 * \brief Get the total number of elements currently stored in the given vector.
 *
 * \param v Pointer to the vector to count the elements of.
 *
 * \return Returns the current number of elements in the vector.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note This returns the number of elements currently stored in the vector; it
 * does not return how much space is available. Use #vec_space() to find how how
 * space is currently available.
 */
unsigned int vec_size(const struct vector *v);

/**
 * \brief Gets the total number of elements that can currently be stored in the
 * buffer.
 *
 * This is the number of elements that the vector can contain before it must be
 * expanded to fit more elements.
 *
 * \param v Pointer to the vector to count the total spaces of.
 *
 * \return Returns the total number of elements that can be stored in the vector
 * before it must be expanded.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note Keep in mind this returns the number of elements that can fit in the
 * buffer, not the number of bytes. This doesn't generally need to be called,
 * because the vector will be expanded automatically when needed.
 */
unsigned int vec_space(const struct vector *v);

/**
 * \brief Resize the dynamically-allocated buffer holding the elements of the
 * vector.
 *
 * This will be called automatically when needed, so it generally does not need
 * to be called by the user. The buffer of the given vector is reallocated to
 * the given size (in bytes).  Returns a flag indicating if the operation
 * succeeded or not. If the memory allocation failed, returns \c -1. If the
 * allocation succeeded, returns \c 0. Note that this does not check to make
 * sure that the current elements of the buffer will fit in the new size;
 * shrinking it too much will result in data loss.
 *
 * \param v Pointer to the vector that needs to be resized.
 * \param size The new size for the buffer in bytes.
 *
 * \return If the memory allocation succeeds, returns \c 0. If the allocation
 * fails, returns \c -1.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note If the passed size is \c 0, then the buffer will be freed. If the
 * buffer inside the vector is \c NULL, then a new buffer will be allocated
 * automatically.  These are side effects of the \c realloc() library function
 * being used.
 *
 * \note Note that if the size is too small to hold all the elements currently
 * in the buffer, data will be lost. Make sure to keep track of these elements
 * elsewhere if this happens.
 */
int vec_resize(struct vector *v, size_t size);

/**
 * \brief Shrink the vector.
 *
 * Shrink the buffer used by the vector so that it is exactly large enough to
 * hold all of its elements, without any wasted space. No data will be lost by
 * calling this function. If the operation succeeds, returns \c 0. If the
 * operation fails, returns \c -1.
 *
 * \param v Pointer to the vector to shrink.
 *
 * \return Returns a code indicating whether or not the memory allocation
 * succeeded. If the allocation succeeded, returns \c 0. It it failed, returns
 * \c -1.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note It is unlikely (impossible?) that reallocating a buffer to a smaller
 * size will fail, the return value is only included for completeness.
 *
 * \note Also note that this guarentees that adding another element to the
 * vector will result in a new memory allocation; calling this function would
 * thus be wasteful unless you are running low on memory and you don't need to
 * add anything more to the vector.
 */
int vec_shrink(struct vector *v);

/**
 * \brief Determines if the vector is empty.
 *
 * \param v Pointer to the vector to check for emptiness.
 *
 * \return If the number of elements in the vector is \c 0, returns true. Else,
 * returns false.
 *
 * \pre <tt>v != NULL</tt>
 */
int vec_isempty(const struct vector *v);

/**
 * \brief Gets the element stored at the given index.
 *
 * Note that no bounds checking is done, so make sure that the index is valid
 * before calling this function.
 *
 * \param v Pointer to the vector to get an element from.
 * \param i Index of the element to get from the vector.
 *
 * \return Returns the void pointer stored at the given index in the buffer.
 *
 * \pre <tt>v != NULL</tt>
 * \pre <tt>i < vec_size(v)</tt>
 *
 * \note Be careful with this function. No bounds checking is done on the
 * argument, so the returned data might be garbage if the index is out of range.
 * Using an out of range index probably won't segfault, so make sure to bounds
 * check before calling this.
 */
void *vec_get(const struct vector *v, unsigned int i);

/**
 * \brief Set the element at the given index to the new value.
 *
 * This does not add a new element to the vector, it just modifies an existing
 * element. However, no bounds checking is done by this function, so make sure
 * the index is valid before calling this.
 *
 * \param v Pointer to the vector in which to modify an element.
 * \param i Index of the element to modify.
 * \param e New element value to store at the given index.
 *
 * \pre <tt>v != NULL</tt>
 * \pre <tt>i < vec_size(v)</tt>
 *
 * \note This does not do any bounds checking, which can lead to bad behavior if
 * the passed index is beyond the bounds of the buffer.  Make sure to do bounds
 * checking before calling this function.
 *
 * \note Also note that the pointer at the given index will be overwritten; if
 * there is unfreed memory there, make sure to #vec_get() the element before
 * calling this function, or the pointer will be lost forever.
 */
void vec_set(struct vector *v, unsigned int i, void *e);

/**
 * \brief Get the first element in the vector, or \c NULL is the vector is
 * empty.
 *
 * This will explicitly check that the vector is not empty before returning, so
 * there is no way for this function to return garbage.
 *
 * \param v Pointer to the vector from which to get the head.
 *
 * \return If the vector is not empty, returns the void pointer stored at index
 * \c 0 in the vector. If the vector is empty, returns \c NULL.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note This function will do bounds checking; that is, it will explicitly
 * check that the vector is not empty before getting the element. Make sure to
 * check for a \c NULL pointer returned from this function, indicating the
 * vector is empty.
 */
void *vec_head(const struct vector *v);

/**
 * \brief Get the last (highest indexed) element in the vector, or \c NULL if
 * the vector is empty.
 *
 * This function does explicit bounds checking, so it will never return garbage
 * from a valid vector.
 *
 * \param v Pointer to the vector from which to get the last element.
 *
 * \return Returns the last valid element in the vector. This is the element
 * stored in the buffer with the highest index. If the vector is empty, returns
 * \c NULL.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note Does explicit bounds checking, so make sure to check for a \c NULL
 * return value if the vector is empty.
 */
void *vec_tail(const struct vector *v);

/**
 * \brief Adds a new element to the end of the vector.
 *
 * The new element will be at the highest index of the vector. If there is not
 * enough space in the buffer for the new element, then the buffer will be
 * resized so the new element will fit. If this resize fails (i.e. failed memory
 * allocation), then \c -1 is returned and the element is not added; the vector
 * is left unchanged. If the element is successfully added, then the index of
 * the new element is returned.
 *
 * \param v Pointer to the vector to which to add an element.
 * \param e Element to add to the end of the vector.
 *
 * \return If the element is successfully added, returns the index of the new
 * element. If the vector needs to be resized to fit the element, and the resize
 * fails, returns \c -1.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note The state of the vector is not changed if the resize operation fails.
 * Also note that the returned index is signed, whereas it should normally be an
 * unsigned number for the other vector operations.
 */
int vec_push(struct vector *v, void *e);

/**
 * \brief Removes the last (highest indexed) element from the vector, returning
 * its value.
 *
 * If the vector is empty, returns \c NULL and does not change the state of the
 * vector. Note that the vector will not be resized, no matter how many elements
 * are popped, unless #vec_shrink() is called.
 *
 * \param v Pointer to the vector from which to pop an element.
 *
 * \return Returns the tail of the vector. This is the element in the buffer
 * with the highest index.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note Make sure to check for a \c NULL return value if the vector is empty.
 * If you want to reclaim memory after a lot of elements are popped, call the
 * #vec_shrink() function.
 */
void *vec_pop(struct vector *v);

/**
 * \brief Deletes an element from the vector.
 *
 * This function will remove the element at the given index from the vector,
 * returning the element so that it is not lost. This operation will then shift
 * the buffer to overwrite the deleted element. This shift is needed in order to
 * keep the vector dense while preserving the order of the elements. Note that
 * this will change the index of every element after the given element. If the
 * given index was not valid (i.e. it is too large), then \c NULL is returned
 * and the vector is unchanged.
 *
 * \param v Pointer to the vector from which to delete an element.
 * \param i Index of the element to delete.
 *
 * \return Returns the deleted element, or \c NULL if the given index was out of
 * range.
 *
 * \pre <tt>v != NULL</tt>
 * \pre <tt>i < vec_size(v)</tt>
 *
 * \note This is a slow and ugly function because of the large memory movement.
 * It also messes with the indices of the vector in order to keep it dense and
 * ordered. You probably shouldn't use this function unless you really have to.
 */
void *vec_delete(struct vector *v, unsigned int i);

/**
 * \brief Apply an operator to each element in a vector.
 *
 * Replace each element with the return value from the operator. The operator
 * takes the element, its index, and a scratch pointer as arguments. This can be
 * used to implement a wide range of iterators over the vector.
 *
 * \param v Pointer to the vector to iterate over.
 * \param op Operation to perform on each element.
 * \param scratch Scratch pointer to pass as an argument to the operation.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note This is the preferred way of iterating over the vector, as using this
 * does not make assumptions about the internals of the vector. See the
 * documentation for the \c VecOperator type for more information about the
 * format of the operator.
 */
void vec_map(struct vector *v, VecOperator op, void *scratch);

/**
 * \brief Sorts the data in the vector according to the given comparison
 * function.
 *
 * The vector will be sorted in ascending order according to the compare
 * function. The compare function should return a value less than, equal to, or
 * greater than zero if the first argument is less than, equal to, or greater
 * than the second argument, respectively.
 *
 * \param v Pointer to the vector to sort.
 * \param compar Comparison function taking as arguments pointers to elements in
 *        the vector, returning a value indicating the ordering of the elements.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note Uses the underlying \c qsort() function in the standard library. The
 * #vec_sort() function is just a wrapper around this call, so it can probably
 * be inlined.
 *
 * \note Be careful with the \p compar argument. The arguments are of type
 * <tt>void *</tt>, but these are pointers to the elements in the vector. So
 * they are really <tt>void **</tt>.
 */
void vec_sort(struct vector *v, int (*compar)(const void *, const void *));

/**
 * \brief Swap two elements in the vector.
 *
 * This will set the value at index \p i to the value at index \p j, and vice
 * versa.
 *
 * \param v Pointer to the vector in which to switch elements.
 * \param i Index of the first element to swap.
 * \param j Index of the second element to swap.
 *
 * \pre <tt>v != NULL</tt>
 * \pre <tt>i < vec_size(v)</tt>
 * \pre <tt>j < vec_size(v)</tt>
 *
 * \note Swaps using a temporary variable.
 */
void vec_swap(struct vector *v, unsigned int i, unsigned int j);


#endif /* end of include guard: _VECTOR_H_ */
