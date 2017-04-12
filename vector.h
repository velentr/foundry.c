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
 * \file vector.h
 *
 * \brief Dynamically-resizable generic arrays.
 *
 * Implementation for dynamically-resizable arrays of similar objects. The
 * interface for this datastructure is similar to STL vectors, but without
 * templates or classes. In many cases, optimizes for speed rather than checking
 * inputs; garbage in, garbage out.
 *
 * Vectors using these functions are assumed to be dense. That is, there are no
 * gaps in between the valid elements in the vector. Also note that the order of
 * the elements within the vector is guarenteed to be constant, while the
 * indices are subject to change (e.g. if an element is deleted).
 *
 * \author Brian Kubisiak
 *
 * \copyright This is free and unencumbered software released into the public
 * domain.
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_


#include <assert.h>
#include <stddef.h>
#include <string.h>

/**
 * Used as the default length for a new vector if a size is not specified in the
 * init function.
 */
#define DEF_VEC_LEN    32



/**
 * \brief Array-like data type that can store a set of objects.
 *
 * This structure contains all the variables and memory necessary for storing a
 * dynamically-resizable array of objects. This is used mainly when the number
 * of elements in the array is not known beforehand, or it is desirable to allow
 * the array to grow with the data.
 *
 * When using a vector, first call the #vec_init() function to allocate a new
 * buffer for the data. Then, any of the \c vec_* functions may be called to
 * operate on the vector.
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
    char *data;         /**< Dynamically-allocated array for holding data. */
    void *(*realloc)(void *, size_t); /**< Allocator for getting more memory. */
    size_t len;         /**< Count of the number of elements in the vector. */
    size_t elemsize;    /**< Size of each element in the vector. */
    size_t space;       /**< Number of elements that can currently fit in the
                             allocated memory. */
};


/**
 * \brief Function that operates on each element of a vector.
 *
 * This indicates a pointer to a function that can be mapped over a vector.
 * Takes in an element, the element's index, and a user-supplied argument.  It
 * can be used to implement a wide variety of filters, iterators, or
 * accumulators. See the #vec_map() function for details of how this is used.
 *
 * \param e Element to operate on.
 * \param i Index of the element being operated on.
 * \param scr Scratch pointer passed to the function unmodified by the
 *        #vec_map() function. This is used to pass other arguments to the
 *        operator.
 *
 * \note If a value needs to be returned outside the #vec_map() function (e.g.
 *       if you are summing the elements), then use the scratch pointer to hold
 *       the return value.
 */
typedef void (*VecOperator)(void *e, unsigned int i, void *scr);


/*
 * Vector Operations
 */

int vec_init(struct vector *v, size_t elemsize, size_t len,
        void *(*alloc)(void *, size_t));
int vec_resize(struct vector *v, size_t size);
void *vec_head(const struct vector *v);
void *vec_tail(const struct vector *v);
int vec_push(struct vector *v, const void *e);
int vec_pop(struct vector *v);
void vec_map(struct vector *v, VecOperator op, void *scratch);
void vec_swap(struct vector *v, size_t i, size_t j);
void vec_free(struct vector *v);
size_t vec_len(const struct vector *v);
size_t vec_space(const struct vector *v);
int vec_shrink(struct vector *v);
int vec_isempty(const struct vector *v);
void *vec_get(const struct vector *v, size_t i);
void vec_set(struct vector *v, size_t i, const void *e);


#endif /* end of include guard: _VECTOR_H_ */

