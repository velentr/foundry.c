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
 * \file list.h
 *
 * \author Brian Kubisiak
 *
 * \brief Doubly-linked list.
 *
 * Implementation for a doubly-linked list with no internal memory allocation.
 * All memory allocation is performed by adding #list_elem structures to
 * existing datatypes; this allows all memory allocation to be completely
 * external.
 */

#ifndef _LIST_H_
#define _LIST_H_


#include <stddef.h>

/**
 * \brief Node in a doubly-linked list structure.
 *
 * The list element can be embedded into another structure in order to create a
 * linked list of said structure. Use #containerof() to get a pointer to the
 * containing structure.
 */
struct list_elem
{
    struct list_elem *prev; /**< Pointer to the previous element in the list. */
    struct list_elem *next; /**< Pointer to the next element in the list. */
};

/**
 * \brief Doubly-linked list structure.
 *
 * The list contains a sentinal list element that is used for containing
 * pointers to the first and last elements of the list. It is used so that
 * iteration is simple to implement, and no special cases are needed when adding
 * elements to an empty list.
 */
struct list
{
    struct list_elem sentinal; /**< List element that is both before the first
                                    element and after the last element. */
};


void list_init(struct list *l);
void list_insert(struct list_elem *prev, struct list_elem *to_add);
struct list_elem *list_remove(struct list_elem *e);
struct list_elem *list_popfront(struct list *l);
struct list_elem *list_popback(struct list *l);
size_t list_size(const struct list *l);
void list_cat(struct list *dst, struct list *src);
struct list_elem *list_head(const struct list *l);
struct list_elem *list_tail(const struct list *l);
struct list_elem *list_begin(const struct list *l);
struct list_elem *list_next(const struct list_elem *e);
struct list_elem *list_prev(const struct list_elem *e);
struct list_elem *list_end(const struct list *l);
void list_pushfront(struct list *l, struct list_elem *e);
void list_pushback(struct list *l, struct list_elem *e);
int list_isempty(const struct list *l);


#endif /* end of include guard: _LIST_H_ */

