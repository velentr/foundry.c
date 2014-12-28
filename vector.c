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
 * vector.c
 *
 * Dynamically-resizable generic arrays, implementation.
 *
 * This file contains the implementation of functions that operate on the
 * dynamically-resizable vector data type. Generally optimizes for speed rather
 * than for validating user input; garbage in, garbage out.
 *
 * Vectors using these functions are assumed to be dense. That is, there are no
 * spaces in between valid elements.
 *
 * Local Functions:
 *      _checkspace -- Increase the amount of space available in the vector if
 *                     all the space is occupied by elements.
 *
 * Functions:
 *      vec_init    -- Initialize the memory for the vector. Must be called
 *                     before the vector can be used.
 *      vec_free    -- Free all the memory associated with the vector.
 *      vec_size    -- Gets the current number of elements stored in the vector.
 *      vec_space   -- Returns the number of elements that can currently be
 *                     stored in the vector. The vector will automatically
 *                     expand past this limit as needed.
 *      vec_resize  -- Resize the vector to the given size. This will be done
 *                     automatically as necessary, so it shouldn't generally
 *                     need to be called.
 *      vec_shrink  -- Shrink the vector so that it contains the absolute
 *                     minimum amount of memory needed to store all the
 *                     elements.
 *      vec_isempty -- Determines if the vector is empty (does not contain any
 *                     elements).
 *      vec_get     -- Get the element at the given index.
 *      vec_set     -- Set the element at the given index to the given value.
 *      vec_head    -- Get the first element in the vector.
 *      vec_tail    -- Get the last element in the vector.
 *      vec_push    -- Add an element to the end of the vector.
 *      vec_pop     -- Remove an element from the end of the vector.
 *      vec_delete  -- Get the element at the given index, and remove it from
 *                     the vector.
 *      vec_map     -- Apply a function to each element in the vector.
 *      vec_sort    -- Sort the data in the vector.
 *      vec_swap    -- Swap two elements in the vector.
 */

#include <stdlib.h>
#include <string.h>

#include "vector.h"

/*
 * _checkspace
 *
 * Description: Check the space of the vector to ensure that another element can
 *              be added. If there is not enough space to add another element,
 *              then resize the vector. If the resize fails, returns -1 without
 *              changing the vector. Else, returns 0.
 *
 * Arguments:   v   Pointer to the vector of which to check the size.
 *
 * Returns:     Returns 0 if the vector currently contains enough space to add
 *              another element at the tail (or was successfully resized to
 *              create enough space). Returns -1 is a resize was attempted, but
 *              did not succeed.
 *
 * Notes:       This function is used so that the vector will grow according to
 *              some predefined rule. Any function that needs to expand the
 *              vector should use this function rather than calling resize
 *              directly.
 *
 *              Currently, doubles the amount of space in the vector if it is
 *              needed.
 */
static int _checkspace(struct vector *v);

/*
 * Initialize the memory for the vector, and allocate a new buffer for storing
 * the data. The size can be specified using the 'size' argument; if 0 is
 * passed, then the default size 'DEF_VEC_SIZE' will be used.
 */
int vec_init(struct vector *v, size_t size)
{
    v->size = 0;        /* We always start off storing 0 elements. */

    if (size == 0)      /* If the passed size is 0, use the default. */
    {
        /*
         * The amount of space defaults to 'DEF_VEC_SIZE' elements if the size
         * is unspecified. Store this in the vector.
         */
        v->space = DEF_VEC_SIZE;

        /*
         * The size variable is used for allocating memory for the vector; make
         * sure to set it to the proper value. Should be the number of bytes for
         * the buffer.
         */
        size = DEF_VEC_SIZE * VEC_ELEMSIZE;
    }
    /*
     * If size was specified, then this should be used for allocating the
     * memory. Calculate the number of elements that can fit in the buffer, but
     * do not change size.
     */
    else
    {
        /*
         * Since size must contains the exact number of value of bytes, it
         * should not be modified. Calculate the number of elements that can be
         * stored in a buffer of 'size' bytes. Note that after this, because of
         * integer division, 'v->space * VEC_ELEMSIZE' is not necessarily equal
         * to 'size'. Some bytes might be wasted.
         */
        v->space = size / VEC_ELEMSIZE;
    }

    /*
     * Allocate the correct amount of memory based on size. Note that if size
     * was not specified (passed as 0), then the above code should have set it
     * to the proper value. If size was specified, then it should be unchanged,
     * even it is not an integer multiple of 'VEC_ELEMSIZE'. Note that since
     * 'realloc' acts like 'malloc' when a NULL pointer is passed, we can reuse
     * code by just resizing the vector.
     */
    v->data = NULL;

    /*
     * The 'vec_resize' function will check for allocation errors. This saves a
     * little bit of repeated code.
     */
    return vec_resize(v, size);
}

/*
 * Free the dynamically-allocated memory associated with this buffer. This uses
 * a call to the 'vec_resize' function so that all memory management is done in
 * only one function. Note that this works because calling realloc with size 0
 * results in a free. The return value will probably be -1, but we can just
 * ignore that.
 */
void vec_free(struct vector *v)
{
    vec_resize(v, 0);
}

/*
 * Get the number of elements currently in the vector. This is stored in the
 * structure, so we just have to return the proper member.
 */
unsigned int vec_size(const struct vector *v)
{
    return v->size;
}

/*
 * Get the amount of space in the vector. This returns the number of elements
 * that can currently be held in the vector without resizing the buffer. It does
 * not give the size in bytes, and is not affected by the number of elements
 * currently in the buffer.
 */
unsigned int vec_space(const struct vector *v)
{
    return v->space;
}

/*
 * Resize the vector to the given size. Note that the passed size is in bytes.
 * This is implemented using realloc, so if the buffer is NULL, this acts like
 * malloc. If the size is 0, then this acts like free. The return code is 0 if
 * the call succeeds, or -1 if the memory allocation fails.
 *
 * NOTE THAT ALL MEMORY MANAGEMENT SHOULD BE DONE THROUGH THIS FUNCTION, for the
 * sake of simplicity.
 */
int vec_resize(struct vector *v, size_t size)
{
    int rc;     /* Stores the return code inidicating success or failure. */

    void *blk;  /* Temporary storage for the new memory block to avoid memory
                   leaks if realloc fails. */

    blk = (void *)realloc(v->data, size);   /* Try to allocate a new buffer. */

    /*
     * Check to see if the allocation succeeded. If it failed, need to return -1
     * while preserving the original buffer.
     */
    if (blk == NULL)    /* Check to see if the allocation failed. */
    {
        rc = -1;        /* If it failed, need to return -1 and preserve the old
                           buffer. */
    }
    else
    {
        /*
         * If it succeeded, store the new buffer, record the new size, and
         * return 0. Note that the new recorded 'space' is the new size divided
         * by the size of each element.
         */
        v->data = blk;
        v->space = size / VEC_ELEMSIZE;
        rc = 0;
    }

    return rc;
}

/*
 * Resize the given vector so that it contains only enough memory to hold all of
 * its elements. This is done by calling the 'vec_resize' function with the
 * vector's current size as the argument.
 */
int vec_shrink(struct vector *v)
{
    /* New size is the number of elements times the size of each element. */
    return vec_resize(v, vec_size(v) * VEC_ELEMSIZE);
}

/*
 * Return a boolean indicating if the vector is empty. This can be done simply
 * by checking if the size (number of elements) is 0.
 */
int vec_isempty(const struct vector *v)
{
    return (vec_size(v) == 0);
}

/*
 * Just return the requested element from the vector's buffer. Does not do any
 * bounds checking, so be careful.
 */
void *vec_get(const struct vector *v, unsigned int i)
{
    return v->data[i];
}

/*
 * Set the element at the given index to the given value. Does not do any bounds
 * checks, and does not resize the vector to fit more data.
 */
void vec_set(struct vector *v, unsigned int i, void *e)
{
    v->data[i] = e;
}

/*
 * Gets the first element of the vector, or NULL if the vector is empty. Makes
 * sure that the vector has valid data before trying to access it, because
 * 'vec_get' does not do bounds checks.
 */
void *vec_head(const struct vector *v)
{
    void *head;                 /* Pointer for storing the return value, so
                                   there is just one return statement. */

    /*
     * Check to see if the vector is empty. Note that the element accessor
     * function does not bounds check, so we need to do that here.
     */
    if (vec_isempty(v))
    {
        /*
         * If the vector is empty, then we need to return a NULL pointer.
         */
        head = NULL;
    }
    else
    {
        /*
         * If the vector is not empty, then get the first element using the
         * proper accessor, and return it.
         */
        head = vec_get(v, 0);
    }

    /*
     * The return value should have been set depending on if the function is
     * empty or not. Just return it here.
     */
    return head;
}

/*
 * Return the last valid element in the vector, or NULL if the vector is empty.
 * Note that the bounds check is used because the vec_get function does not do
 * any bounds checking.
 */
void *vec_tail(const struct vector *v)
{
    void *tail;                 /* Pointer for storing the return value, so
                                   there is just one return statement. */

    /*
     * Check to see if the vector is empty. Note that the element accessor
     * function does not bounds check, so we need to do that here.
     */
    if (vec_isempty(v))
    {
        /*
         * If the vector is empty, then we need to return a NULL pointer.
         */
        tail = NULL;
    }
    else
    {
        /*
         * If the vector is not empty, then get the last element using the
         * proper accessor, and return it. Note that the last element is at the
         * index 'size - 1' because the buffer is zero-indexed.
         */
        tail = vec_get(v, vec_size(v) - 1);
    }

    /*
     * The return value should have been set depending on if the function is
     * empty or not. Just return it here.
     */
    return tail;
}

/*
 * Add a new element to the end of the vector. First, makes sure that there is
 * enough space for the new element. Then, if there is enough space, adds the
 * element to the end of the buffer.
 */
int vec_push(struct vector *v, void *e)
{
    int rc;                 /* Return code indicating if the element was added.
                               This is an indicator of whether or not the resize
                               failed. */

    /*
     * Make sure that there is enough space for the new element in the vector,
     * resizing the vector if needed. If this fails, then the new element cannot
     * be added to the vector.
     */
    rc = _checkspace(v);

    if (rc == 0)            /* Check if the resize operation succeeded. */
    {
        /*
         * If the resize did succeed, then there is enough space in the buffer
         * for the new element. Add the element to the tail and increment the
         * count. Note that 'size - 1' is used because the buffer is
         * zero-indexed.
         */
        v->size++;
        vec_set(v, vec_size(v) - 1, e);
    }

    return rc;              /* Return code was set by resize. Just return it. */
}

/*
 * Remove the last item from the vector and return its value. This does not
 * actually overwrite the data in the buffer; it just decrements the size of the
 * vector. Since the vector is assumed to be dense, this has the same effect of
 * saying that the last element is no longer valid.
 */
void *vec_pop(struct vector *v)
{
    void *elem;             /* Temporary storage for the element to return. */

    /*
     * Get the last element in the vector (the tail). This call will return NULL
     * if the vector is empty. In this case, no element will be removed.
     */
    elem = vec_tail(v);

    /*
     * If the tail was not NULL (i.e. the vector is not empty), decrement the
     * size to indicate that an element was removed.
     */
    if (elem != NULL)
    {
        v->size--;
    }

    return elem;            /* Already have tail element; just return it. */
}

/*
 * Remove a single element from the vector, without changing the order of the
 * elements, while keeping the vector dense. This involves shifting the buffer
 * to overwrite the deleted element, so it can be pretty inefficient. Make sure
 * to keep a copy of the deleted pointer so it can be returned.
 */
void *vec_delete(struct vector *v, unsigned int i)
{
    void *elem;         /* Temporary storage for the deleted element. */
    void *src, *dest;   /* Pointers used for the call to memmove. */
    size_t n;           /* Number of bytes that need to be moved. */

    /*
     * First, check to make sure that the given index is within the bounds of
     * the vector.
     */
    if (i >= vec_size(v))
    {
        elem = NULL;    /* If this is not a valid index, just return NULL. */
    }
    /*
     * If the index is valid, need to get the element, then shift all the other
     * elements forward one space in order to keep the vector dense.
     */
    else
    {
        elem = vec_get(v, i);   /* Temporarily store the element so that it can
                                   be returned. */

        /*
         * The memory must be moved into the location of the deleted element;
         * this is the destination of the move. Get a pointer to that location
         * here. The source of the move is the pointer to the next element in
         * the buffer, so that the order of the elements is not changed.
         */
        dest = &(v->data[i]);
        src = &(v->data[i + 1]);

        /*
         * The number of elements to move is the highest valid index in the
         * vector, minus the index of the element being removed. Multiply by
         * VEC_ELEMSIZE to get the number of bytes to move.
         */
        n = ((vec_size(v) - 1) - i) * VEC_ELEMSIZE;

        memmove(dest, src, n);  /* Shift the buffer, overwriting the deleted
                                   element while preserving the order. */

        v->size--;              /* Record that an element was removed. */
    }

    return elem;                /* Element was stored above; just return it. */
}

/*
 * Apply an operation to each element in the vector, replacing the element with
 * the return value from the operation. The operation takes the element, the
 * element index, and a scratch pointer as arguments.
 */
void vec_map(struct vector *v, VecOperator op, void *scratch)
{
    void *elem;     /* New element to be stored at each location. */
    int i;          /* Loop index counter. */

    /*
     * Loop over every element in the vector, performing the operation on each
     * element and storing the result. Write the new value back into the vector.
     */
    for (i = 0; i < vec_size(v); i++)
    {
        /* Transform each element with the given operation. */
        elem = op(vec_get(v, i), i, scratch);

        /* Store the new value for each element. */
        vec_set(v, i, elem);
    }
}

/*
 * Check to make sure there is enough space in the vector for adding another
 * element. If there is not enough space, double the size of the buffer.
 */
static int _checkspace(struct vector *v)
{
    int rc;         /* Return code for the resize operation. */

    /*
     * Check if the vector is full (i.e. the buffer cannot fit another element
     * without resizing).
     */
    if (vec_size(v) == vec_space(v))
    {
        /* If it is full, double the space used by the buffer. */
        rc = vec_resize(v, 2 * vec_size(v) * VEC_ELEMSIZE);
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

/*
 * Sort the data in the vector using the standard library qsort function. The
 * arguments to the qsort function are pretty easy to get from the vector.
 */
void vec_sort(struct vector *v, int (*compar)(const void *, const void *))
{
    /*
     * Sort the data. This is a vector of vec_size(v) elements, each with size
     * VEC_ELEMSIZE. The comparison function can just be passed through.
     */
    qsort(v->data, vec_size(v), VEC_ELEMSIZE, compar);
}

/*
 * Swap two of the elements by first storing one in a temporary variable, then
 * moving them around.
 */
void vec_swap(struct vector *v, unsigned int i, unsigned int j)
{
    void *tmp;                      /* Temporary storage for element i. */

    /* Get element i, and store it in a temporary variable. */
    tmp = vec_get(v, i);

    /*
     * Set the ith  element to the value of the jth element. The ith element is
     * still stored in tmp.
     */
    vec_set(v, i, vec_get(v, j));

    /* Now move the temporary value into the jth element. */
    vec_set(v, j, tmp);
}
