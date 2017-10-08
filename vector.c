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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

/* Check if the vector has enough space to grow. */
static int _vec_checkspace(struct vector *v);

/* Initialize a vector. */
int vec_init(struct vector *v, size_t elemsize, size_t len,
        void *(*alloc)(void *, size_t))
{
    size_t size;

    assert(v != NULL);
    assert(elemsize != 0);

    /* We always start off storing 0 elements. */
    v->len = 0;
    v->elemsize = elemsize;

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
    v->realloc = alloc;

    return vec_resize(v, size);
}

/* Resize the buffer used by the vector. */
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

/* Get the first element of the vector. */
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

/* Get the last element in the vector. */
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

/* Adds a new element to the end of the vector. */
int vec_push(struct vector *v, const void *e)
{
    int rc;
    size_t idx;

    assert(v != NULL);

    /* Make sure that there is enough space for the new element in the vector,
     * resizing the vector if needed. If this fails, then the new element cannot
     * be added to the vector.
     */
    rc = _vec_checkspace(v);

    /* If the resize did succeed, then there is enough space in the buffer for
     * the new element. Add the element to the tail and increment the count. The
     * old size is the index of the new element; store in rc so it can be
     * returned.
     */
    if (rc == 0)
    {
        idx = v->len++;
        vec_set(v, idx, e);
        rc = (int)idx;
    }

    return rc;
}

/* Removes the last (highest indexed) element from the vector. */
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

/* Apply an operator to each element in a vector. */
void vec_map(struct vector *v, vec_operator op, void *scratch)
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

/* Swap two elements in the vector. */
void vec_swap(struct vector *v, size_t i, size_t j)
{
    void *buf;
    void *_tmp;

    assert(v != NULL);
    assert(i < vec_len(v));
    assert(j < vec_len(v));

    /* Use the last (empty) element for temporary storage. */
    buf = (void *)(v->data + (v->space - 1)*v->elemsize);

    /* memcpy can't handle overlapping regions, so need to check this here. */
    if (i == j)
        return;

    /* Get element i, and store it in a temporary variable. */
    _tmp = vec_get(v, i);
    memcpy(buf, _tmp, v->elemsize);

    /*
     * Set the ith  element to the value of the jth element. The ith element is
     * still stored in tmp.
     */
    vec_set(v, i, vec_get(v, j));

    /* Now move the temporary value into the jth element. */
    vec_set(v, j, buf);
}

/* Free all the memory associated with the vector. */
void vec_destroy(struct vector *v)
{
    assert(v != NULL);

    vec_resize(v, 0);
}

/* Get the total number of elements currently stored in the given vector. */
size_t vec_len(const struct vector *v)
{
    assert(v != NULL);

    return v->len;
}

 /* Gets the total number of elements that can currently be stored in the
  * buffer.
  */
size_t vec_space(const struct vector *v)
{
    assert(v != NULL);

    return v->space;
}

/* Shrink the vector. */
int vec_shrink(struct vector *v)
{
    assert(v != NULL);

    /* New size is the number of elements times the size of each element. Note
     * that one extra slot is allocated, as it is needed by vec_swap().
     */
    return vec_resize(v, (vec_len(v) + 1) * v->elemsize);
}

/* Determines if the vector is empty. */
int vec_isempty(const struct vector *v)
{
    assert(v != NULL);

    return (vec_len(v) == 0);
}

/* Gets the element stored at the given index. */
void *vec_get(const struct vector *v, size_t i)
{
    assert(v != NULL);
    assert(i < vec_len(v));

    return (void *)(v->data + i*v->elemsize);
}

/* Set the element at the given index to the new value. */
void vec_set(struct vector *v, size_t i, const void *e)
{
    assert(v != NULL);
    assert(i < vec_len(v));

    memcpy(v->data + i*v->elemsize, e, v->elemsize);
}

/* Check to make sure there is enough space in the vector for adding another
 * element. If there is not enough space, double the size of the buffer.
 */
static int _vec_checkspace(struct vector *v)
{
    int rc;

    assert(v != NULL);

    /*
     * Check if the vector is full (i.e. the buffer cannot fit another element
     * without resizing).
     */
    if (vec_len(v) == vec_space(v) - 1)
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

