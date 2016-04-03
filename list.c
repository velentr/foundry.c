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

/*
 * list.c
 *
 * Doubly-linked list, implementation.
 */

#include <assert.h>
#include <stdlib.h>

#include "list.h"

/* Initialize the given list. */
void list_init(struct list *l)
{
    assert(l != NULL);

    /* Set the prev and next pointers of the sentinal equal to the sentinal
     * itself. This indicates that the list is empty, and prevents any special
     * cases when adding to an empty list.
     */
    l->sentinal.prev = l->sentinal.next = &l->sentinal;
}

/* Get the first element of the list, for iteration. */
struct list_elem *list_begin(struct list *l)
{
    /* When iterating forwards, the beginning is the first valid (non-sentinal
     * element in the list. */
    return list_head(l);
}

/* Get the next element in the list, for iteration. */
struct list_elem *list_next(struct list_elem *e)
{
    assert(e != NULL);

    return e->next;
}

/* Get the previous element in the list, for iteration. */
struct list_elem *list_prev(struct list_elem *e)
{
    assert(e != NULL);

    return e->prev;
}

/* Get the sentinal value that indicates the list iteration is complete. */
struct list_elem *list_end(struct list *l)
{
    assert(l != NULL);

    return &l->sentinal;
}

/* Get the first element from the list. */
struct list_elem *list_head(struct list *l)
{
    assert(l != NULL);

    /* Get the first valid (non-sentinal) element in the list, and return it. */
    return l->sentinal.next;
}

/* Get the last element from the list. */
struct list_elem *list_tail(struct list *l)
{
    assert(l != NULL);

    return l->sentinal.prev;
}

/* Insert the new element immediately following the given old element. */
void list_insert(struct list_elem *old, struct list_elem *new)
{
    assert(old != NULL);
    assert(new != NULL);

    /* Update the pointers in 'new'. */
    new->next = old->next;
    new->prev = old;

    /* Insert 'new' immediately following 'old'. */
    old->next->prev = new;
    old->next = new;
}

/* Insert the given element at the front of the list. */
void list_pushfront(struct list *l, struct list_elem *new)
{
    assert(l != NULL);

    list_insert(&l->sentinal, new);
}

/* Insert the given element at the back of the list. */
void list_pushback(struct list *l, struct list_elem *new)
{
    assert(l != NULL);
    assert(l->sentinal.prev != NULL);

    list_insert(l->sentinal.prev, new);
}

/* Remove the given element from its list. */
struct list_elem *list_remove(struct list_elem *e)
{
    assert(e != NULL);

    /* Update the pointers in the next and previous elements. */
    e->prev->next = e->next;
    e->next->prev = e->prev;

    return e;
}

/* Remove the first element in the list, and return it. */
struct list_elem *list_popfront(struct list *l)
{
    struct list_elem *e;

    assert(!list_empty(l));

    /* Get the first element and remove it. */
    e = list_head(l);
    return list_remove(e);
}

/* Remove the last element in the list, and return it. */
struct list_elem *list_popback(struct list *l)
{
    struct list_elem *e;

    assert(!list_empty(l));

    /* Get the last element and remove it. */
    e = list_tail(l);
    return list_remove(e);
}

/* Return the number of elements in the list. */
size_t list_size(struct list *l)
{
    struct list_elem *e;
    size_t num = 0;

    /* Iterate over all the elements in the list. */
    for (e = list_begin(l); e != list_end(l); e = list_next(e))
    {
        num++;
    }

    return num;
}

/* Check if the given list contains any elements. */
int list_empty(struct list *l)
{
    assert(l != NULL);

    return (l->sentinal.prev == &l->sentinal);
}

