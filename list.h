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
 * list.h
 *
 * Doubly-linked list, public interface.
 *
 * Contains the public interface for doubly-linked list with no internal memory
 * allocation. All memory allocation is performed by adding list_elem structures
 * to existing datatypes; this allows all memory allocation to be completely
 * external.
 *
 * Constants:
 *
 * Datatypes:
 *
 * Functions:
 *
 *  list_init
 *
 *  Traversal:
 *      list_begin
 *      list_next
 *      list_prev
 *      list_end
 *      list_head
 *      list_tail
 *  Insertion:
 *      list_insert
 *      list_pushfront
 *      list_pushback
 *  Removal:
 *      list_remove
 *      list_popfront
 *      list_popback
 *  Properties:
 *      list_size
 *      list_empty
 *
 * Macros:
 *  list_entry      Gets the element containing the given list node.
 */

#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h>

#define list_entry(LIST_ELEM, STRUCT, MEMBER) ((STRUCT *)((size_t) (LIST_ELEM) \
            - offsetof(STRUCT, MEMBER)))


/*
 * struct list_elem
 *
 * Node in a doubly-linked list structure.
 *
 * Members: prev    Pointer to the previous element in the list.
 *          next    Pointer to the next element in the list.
 */
struct list_elem
{
    struct list_elem *prev;
    struct list_elem *next;
};

/*
 * struct list
 *
 * Doubly-linked list structure.
 *
 * Members: sentinal    List element that is both before the first element and
 *                      after the last element. Used for iterating over the
 *                      list, and to avoid special cases.
 */
struct list
{
    struct list_elem sentinal;
};


void list_init(struct list *l);
struct list_elem *list_begin(struct list *);
struct list_elem *list_next(struct list_elem *);
struct list_elem *list_prev(struct list_elem *);
struct list_elem *list_end(struct list *);
struct list_elem *list_head(struct list *);
struct list_elem *list_tail(struct list *);

void list_insert(struct list_elem *old, struct list_elem *new);
void list_pushfront(struct list *, struct list_elem *);
void list_pushback(struct list *, struct list_elem *);

struct list_elem *list_remove(struct list_elem *);
struct list_elem *list_popfront(struct list *);
struct list_elem *list_popback(struct list *);

size_t list_size(struct list *);
int list_empty(struct list *);

#endif /* end of include guard: _LIST_H_ */

