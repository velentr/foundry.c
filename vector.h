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

#ifndef _VECTOR_H_
#define _VECTOR_H_


#include <assert.h>
#include <stddef.h>
#include <string.h>

/* Default vector length. */
#define DEF_VEC_LEN    32


struct vector
{
    char *data;
    void *(*realloc)(void *, size_t);
    size_t len;
    size_t elemsize;
    size_t space;
};


/* Function for operating on elements of a vector. */
typedef void (*vec_operator)(void *e, size_t i, void *scratch);


/*
 * Vector Operations
 */

/* Initialize a vector. */
int vec_init(struct vector *v, size_t elemsize, size_t len,
        void *(*alloc)(void *, size_t));
/* Resize a vector. */
int vec_resize(struct vector *v, size_t size);
/* Get the first element of a vector. */
void *vec_head(const struct vector *v);
/* Get the last element of a vector. */
void *vec_tail(const struct vector *v);
/* Add an element to the end of a vector. */
int vec_push(struct vector *v, const void *e);
/* Remove an element from the tail of the vector. */
int vec_pop(struct vector *v);
/* Operate on each element in the vector. */
void vec_map(struct vector *v, vec_operator op, void *scratch);
/* Swap two elements in the vector. */
void vec_swap(struct vector *v, size_t i, size_t j);
/* Deallocate a vector. */
void vec_destroy(struct vector *v);
/* Get the number of elements in a vector. */
size_t vec_len(const struct vector *v);
/* Get the amount of memory used by the vector. */
size_t vec_space(const struct vector *v);
/* Shrink a vector to fit only the elements it contains. */
int vec_shrink(struct vector *v);
/* Check if a vector is empty. */
int vec_isempty(const struct vector *v);
/* Get an element from a vector. */
void *vec_get(const struct vector *v, size_t i);
/* Set an element of a vector. */
void vec_set(struct vector *v, size_t i, const void *e);


#endif /* end of include guard: _VECTOR_H_ */

