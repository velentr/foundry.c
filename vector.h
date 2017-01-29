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
int vec_push(struct vector *v, void *e);
int vec_pop(struct vector *v);
void vec_map(struct vector *v, VecOperator op, void *scratch);
void vec_swap(struct vector *v, size_t i, size_t j);

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
static inline void vec_free(struct vector *v)
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
static inline size_t vec_len(const struct vector *v)
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
static inline size_t vec_space(const struct vector *v)
{
    assert(v != NULL);

    return v->space;
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
static inline int vec_shrink(struct vector *v)
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
static inline int vec_isempty(const struct vector *v)
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
static inline void *vec_get(const struct vector *v, size_t i)
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
static inline void vec_set(struct vector *v, size_t i, void *e)
{
    assert(v != NULL);
    assert(i < vec_len(v));

    memcpy(v->data + i*v->elemsize, e, v->elemsize);
}


#endif /* end of include guard: _VECTOR_H_ */
