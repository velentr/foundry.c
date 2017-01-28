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
 * \file vector.c
 *
 * \brief Dynamically-resizable generic arrays, implementation.
 *
 * This file contains the implementation of functions that operate on the
 * dynamically-resizable vector data type. Generally optimizes for speed rather
 * than for validating user input; garbage in, garbage out.
 *
 * Vectors using these functions are assumed to be dense. That is, there are no
 * spaces in between valid elements.
 *
 * \author Brian Kubisiak
 *
 * \copyright This is free and unencumbered software released into the public
 * domain.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

/**
 * \brief Check if the vector has enough space to grow.
 *
 * Check the space of the vector to ensure that another element can be added. If
 * there is not enough space to add another element, then resize the vector. If
 * the resize fails, returns -1 without changing the vector. Else, returns 0.
 *
 * This function is used so that the vector will grow according to some
 * predefined rule. Any function that needs to expand the vector should use this
 * function rather than calling resize directly.
 *
 * Currently, doubles the amount of space in the vector if it is needed.
 *
 * \param [in] v Pointer to the vector of which to check the size.
 *
 * \return Returns 0 if the vector currently contains enough space to add
 * another element at the tail (or was successfully resized to create enough
 * space). Returns -1 is a resize was attempted, but did not succeed.
 *
 * \pre <tt>v != NULL</tt>
 */
static int _checkspace(struct vector *v);

/**
 * \brief Initialize the memory for the vector. Must be called before the vector
 * can be used.
 *
 * Initialize the given vector so that it is ready to be used. This includes
 * allocating the memory for the vector, as well as setting initial values for
 * the member variables. The argument \p v is a pointer to the vector to
 * initialize. The argument \p elemsize is the size of each individual element
 * in the vector. The argument \p len is an initial value for the number of
 * objects in the vector, or \c 0 if the default length is desired. Returns a
 * flag indicating whether or not the operation succeeded.
 *
 * \param v Pointer to the vector to initialize.
 * \param elemsize Size of each element in the vector.
 * \param len Initial value for the number of elements in the vector. If this
 *        argument is \c 0, then the default size will be used.
 * \param alloc Memory allocator used for performing all allocation for the
 *        vector. Interface should be equivalent to \c realloc. Should be set to
 *        \c NULL to use the default allocator.
 *
 * \return If the memory is successfully allocated, returns \c 0. If the memory
 *         allocation fails, returns \c -1.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note The \p len argument shouldn't generally be used unless there is a
 *       specific reason to limit the size of the vector (or if the needed size
 *       of the vector is known beforehand). It should only be used when extreme
 *       performance is needed; you should probably just pass \c 0.
 */
int vec_init(struct vector *v, size_t elemsize, size_t len,
        void *(*alloc)(void *, size_t))
{
    size_t size;

    assert(v != NULL);

    v->len = 0;         /* We always start off storing 0 elements. */

    /* The amount of space defaults to 'DEF_VEC_SIZE' elements if the size is
     * unspecified. Store this in the vector.
     */
    if (len == 0)
    {
        v->space = DEF_VEC_LEN;
        size = DEF_VEC_LEN * elemsize;
    }
    /* If size was specified, then this should be used for allocating the
     * memory. Calculate the number of bytes for the buffer.
     */
    else
    {
        v->space = len;
        size = len * elemsize;
    }

    /* Allocate the correct amount of memory based on size. Note that since
     * 'realloc' acts like 'malloc' when a NULL pointer is passed, we can reuse
     * code by just resizing the vector.
     */
    v->data = NULL;

    /* If no allocator is provided, just use 'realloc'. */
    if (alloc == NULL)
    {
        v->realloc = realloc;
    }
    else
    {
        v->realloc = alloc;
    }

    return vec_resize(v, size);
}

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
void vec_free(struct vector *v)
{
    assert(v != NULL);

    vec_resize(v, 0);
}

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
size_t vec_len(const struct vector *v)
{
    assert(v != NULL);

    return v->len;
}

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
size_t vec_space(const struct vector *v)
{
    assert(v != NULL);

    return v->space;
}

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
int vec_resize(struct vector *v, size_t size)
{
    int rc;     /* Stores the return code inidicating success or failure. */

    void *blk;  /* Temporary storage for the new memory block to avoid memory
                   leaks if realloc fails. */

    assert(v != NULL);

    blk = (void *)v->realloc(v->data, size);

    /* Check to see if the allocation succeeded. If it failed, need to return -1
     * while preserving the original buffer.
     */
    if (blk == NULL)    /* Check to see if the allocation failed. */
    {
        rc = -1;        /* If it failed, need to return -1 and preserve the old
                           buffer. */
    }
    else
    {
        /* If it succeeded, store the new buffer, record the new size, and
         * return 0. Note that the new recorded 'space' is the new size divided
         * by the size of each element.
         */
        v->data = blk;
        v->space = size / v->elemsize;
        rc = 0;
    }

    return rc;
}

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
int vec_shrink(struct vector *v)
{
    assert(v != NULL);

    /* New size is the number of elements times the size of each element. */
    return vec_resize(v, vec_len(v) * v->elemsize);
}

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
int vec_isempty(const struct vector *v)
{
    assert(v != NULL);

    return (vec_len(v) == 0);
}

/**
 * \brief Gets the element stored at the given index.
 *
 * Note that no bounds checking is done, so make sure that the index is valid
 * before calling this function.
 *
 * \param v Pointer to the vector to get an element from.
 * \param i Index of the element to get from the vector.
 *
 * \return Returns a void pointer pointing to the requested object.
 *
 * \pre <tt>v != NULL</tt>
 * \pre <tt>i < vec_len(v)</tt>
 *
 * \note Be careful with this function. No bounds checking is done on the
 * argument, so the returned data might be garbage if the index is out of range.
 * Using an out of range index probably won't segfault, so make sure to bounds
 * check before calling this.
 */
void *vec_get(const struct vector *v, size_t i)
{
    assert(v != NULL);
    assert(i < vec_len(v));

    return (void *)(v->data + i*v->elemsize);
}

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
 * \pre <tt>i < vec_len(v)</tt>
 *
 * \note This does not do any bounds checking, which can lead to bad behavior if
 * the passed index is beyond the bounds of the buffer.  Make sure to do bounds
 * checking before calling this function.
 *
 * \note Also note that the pointer at the given index will be overwritten; if
 * there is unfreed memory there, make sure to #vec_get() the element before
 * calling this function, or the pointer will be lost forever.
 */
void vec_set(struct vector *v, size_t i, void *e)
{
    assert(v != NULL);
    assert(i < vec_len(v));

    memcpy(v->data + i*v->elemsize, e, v->elemsize);
}

/**
 * \brief Get the first element in the vector, or \c NULL is the vector is
 * empty.
 *
 * This will explicitly check that the vector is not empty before returning, so
 * there is no way for this function to return garbage.
 *
 * \param v Pointer to the vector from which to get the head.
 *
 * \return If the vector is not empty, returns a pointer to the element stored
 * at index \c 0 in the vector. If the vector is empty, returns \c NULL.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note This function will do bounds checking; that is, it will explicitly
 * check that the vector is not empty before getting the element. Make sure to
 * check for a \c NULL pointer returned from this function, indicating the
 * vector is empty.
 */
void *vec_head(const struct vector *v)
{
    void *head;

    assert(v != NULL);

    /* Check to see if the vector is empty. Note that the element accessor
     * function does not bounds check, so we need to do that here.
     */
    if (vec_isempty(v))
    {
        head = NULL;
    }
    else
    {
        head = vec_get(v, 0);
    }

    /* The return value should have been set depending on if the function is
     * empty or not. Just return it here.
     */
    return head;
}

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
void *vec_tail(const struct vector *v)
{
    void *tail;

    assert(v != NULL);

    /* Check to see if the vector is empty. Note that the element accessor
     * function does not bounds check, so we need to do that here.
     */
    if (vec_isempty(v))
    {
        tail = NULL;
    }
    else
    {
        /* If the vector is not empty, then get the last element using the
         * proper accessor, and return it. Note that the last element is at the
         * index 'size - 1' because the buffer is zero-indexed.
         */
        tail = vec_get(v, vec_len(v) - 1);
    }

    /* The return value should have been set depending on if the function is
     * empty or not. Just return it here.
     */
    return tail;
}

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
int vec_push(struct vector *v, void *e)
{
    int rc;

    assert(v != NULL);

    /* Make sure that there is enough space for the new element in the vector,
     * resizing the vector if needed. If this fails, then the new element cannot
     * be added to the vector.
     */
    rc = _checkspace(v);

    /* If the resize did succeed, then there is enough space in the buffer for
     * the new element. Add the element to the tail and increment the count. The
     * old size is the index of the new element; store in rc so it can be
     * returned.
     */
    if (rc == 0)
    {
        rc = v->len++;
        vec_set(v, rc, e);
    }

    return rc;
}

/**
 * \brief Removes the last (highest indexed) element from the vector.
 *
 * If the vector is empty, returns -1 and does not change the state of the
 * vector. Note that the vector will not be resized, no matter how many elements
 * are popped, unless #vec_shrink() is called.
 *
 * \param v Pointer to the vector from which to pop an element.
 *
 * \return If the operation succeeds (i.e. the vector is nonempty), returns 0.
 * If the operation fails, returns -1.
 *
 * \pre <tt>v != NULL</tt>
 *
 * \note If you want to reclaim memory after a lot of elements are popped, call
 * the #vec_shrink() function.
 */
int vec_pop(struct vector *v)
{
    int rc;

    assert(v != NULL);

    /* If the vector is empty, do nothing and return -1. */
    if (vec_isempty(v))
    {
        rc = -1;
    }
    /* Else, shrink the vector and return 0. */
    else
    {
        v->len--;
        rc = 0;
    }

    return rc;
}

/**
 * \brief Apply an operator to each element in a vector.
 *
 * The operator takes the element, its index, and a scratch pointer as
 * arguments. This can be used to implement a wide range of iterators over the
 * vector.
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
void vec_map(struct vector *v, VecOperator op, void *scratch)
{
    size_t i;

    assert(v != NULL);

    /*
     * Loop over every element in the vector, performing the operation on each
     * element and storing the result. Write the new value back into the vector.
     */
    for (i = 0; i < vec_len(v); i++)
    {
        /* Transform each element with the given operation. */
        op(vec_get(v, i), i, scratch);
    }
}

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
 * \pre <tt>i < vec_len(v)</tt>
 * \pre <tt>j < vec_len(v)</tt>
 *
 * \note Swaps using a temporary variable. This may be dangerous if there are
 * dangling references to either element in the vector.
 */
void vec_swap(struct vector *v, size_t i, size_t j)
{
    char tmp[v->elemsize];
    void *_tmp;

    assert(v != NULL);
    assert(i < vec_len(v));
    assert(j < vec_len(v));

    /* Get element i, and store it in a temporary variable. */
    _tmp = vec_get(v, i);
    memcpy(&tmp, _tmp, v->elemsize);

    /*
     * Set the ith  element to the value of the jth element. The ith element is
     * still stored in tmp.
     */
    vec_set(v, i, vec_get(v, j));

    /* Now move the temporary value into the jth element. */
    vec_set(v, j, tmp);
}

/*
 * Check to make sure there is enough space in the vector for adding another
 * element. If there is not enough space, double the size of the buffer.
 */
static int _checkspace(struct vector *v)
{
    int rc;

    assert(v != NULL);

    /*
     * Check if the vector is full (i.e. the buffer cannot fit another element
     * without resizing).
     */
    if (vec_len(v) == vec_space(v))
    {
        /* If it is full, double the space used by the buffer. */
        rc = vec_resize(v, 2 * vec_len(v) * v->elemsize);
    }
    else
    {
        /* If it is not full, no need to resize; always succeeds. */
        rc = 0;
    }

    /*
     * Return code was set by the call to vec_resize if the buffer was resized.
     * Otherwise, the code was zeroed.
     */
    return rc;
}

