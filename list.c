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
 * \file list.c
 *
 * \brief Doubly-linked list, implementation.
 */

#include <assert.h>
#include <stdlib.h>

#include "list.h"

/**
 * \brief Initialize a linked list structure.
 *
 * Set the pointers in the given linked list structure to valid initial values.
 * This function should be called before any other functions can be used.
 *
 * \param [in,out] l Pointer to the linked list to initialize.
 *
 * \pre <tt>l != NULL</tt>
 */
void list_init(struct list *l)
{
    assert(l != NULL);

    /* Set the prev and next pointers of the sentinal equal to the sentinal
     * itself. This indicates that the list is empty, and prevents any special
     * cases when adding to an empty list.
     */
    l->sentinal.prev = l->sentinal.next = &l->sentinal;
}

/**
 * \brief Insert a new element into a linked list.
 *
 * Inserts the list element \p to_add immediately after \p prev in a linked
 * list.  Note that \p prev does not have to be in an existing list in order for
 * the function to work.
 *
 * \warning If \p to_add is already in a list \c l, then calling this function
 * will break \c l.
 *
 * \param [in] prev Pointer to a list element after which to insert \p to_add.
 * \param [in] to_add Pointer to a list element to add to the list containing \p
 * prev.
 *
 * \pre <tt>prev != NULL</tt>
 * \pre <tt>to_add != NULL</tt>
 */
void list_insert(struct list_elem *prev, struct list_elem *to_add)
{
    assert(prev != NULL);
    assert(to_add != NULL);

    /* Update the pointers in 'to_add'. */
    to_add->next = prev->next;
    to_add->prev = prev;

    /* Insert 'to_add' immediately following 'prev'. */
    prev->next->prev = to_add;
    prev->next = to_add;
}

/**
 * \brief Remove the given item from its containing list.
 *
 * The list will be relinked so that \p e is removed without breaking the
 * continuity of the list.
 *
 * \warning This function will exhibit undefined behavior if \p e is not already
 * in a list.
 *
 * \param [in] e Element to remove from the list.
 *
 * \pre <tt>e != NULL</tt>
 *
 * \return Returns \p e. This is used for convenience in the internal
 *         implementation.
 */
struct list_elem *list_remove(struct list_elem *e)
{
    assert(e != NULL);

    /* Update the pointers in the next and previous elements. */
    e->prev->next = e->next;
    e->next->prev = e->prev;

    return e;
}

/**
 * \brief Pop an element from the front of a list.
 *
 * Removes the first element from the list \p l and returns it. Can be used with
 * #list_pushfront() to implement a stack, or with #list_pushback() to implement
 * a queue.
 *
 * \warning This will exhibit undefined behavior if \p l is an empty list.
 *
 * \param [in] l Pointer to the list from which to pop the first element.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>!list_isempty(l)</tt>
 *
 * \return Returns a pointer to the element that was popped from \p l.
 */
struct list_elem *list_popfront(struct list *l)
{
    struct list_elem *e;

    assert(l != NULL);
    assert(!list_isempty(l));

    /* Get the first element and remove it. */
    e = list_head(l);
    return list_remove(e);
}

/**
 * \brief Pop an element from the end of a list.
 *
 * Removes the last element from the list \p l and returns it. Can be used with
 * #list_pushback() to implement a stack, or with #list_pushfront() to implement
 * a queue.
 *
 * \warning This will exhibit undefined behavior if \p l is an empty list.
 *
 * \param [in] l Pointer to the list from which to pop the last element.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>!list_isempty(l)</tt>
 *
 * \return Returns a pointer to the element that was popped from \p l.
 */
struct list_elem *list_popback(struct list *l)
{
    struct list_elem *e;

    assert(l != NULL);
    assert(!list_isempty(l));

    /* Get the last element and remove it. */
    e = list_tail(l);
    return list_remove(e);
}

/**
 * \brief Compute the size of the linked list.
 *
 * Counts the number of elements in the list by iterating over all the elements.
 * This function will take O(n) time with respect to the length of the list. Due
 * to the sparse nature of the list, it is impractical to compute the length in
 * constant time.
 *
 * \param [in] l Pointer to the list of which to get the size.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns the number of elements in \p l.
 */
size_t list_size(const struct list *l)
{
    struct list_elem *e;
    size_t num = 0;

    assert(l != NULL);

    /* Iterate over all the elements in the list. */
    for (e = list_begin(l); e != list_end(l); e = list_next(e))
    {
        num++;
    }

    return num;
}

/**
 * \brief Concatenate two lists.
 *
 * Adds the list \p src onto the end of list \p dst such that after the function
 * returns, \p dst will contain all previous elements as well as all elements of
 * \p src. The head of \p dst will remain the same, but the new tail of \p dst
 * will be the tail of \p src. The old tail of \p dst will be connected to the
 * head of \p src. Note that \p src will be invalid after this operation.
 *
 * \param [out] dst List to which \p src is appended.
 * \param [in] src List to append onto \p dst.
 */
void list_cat(struct list *dst, struct list *src)
{
    struct list_elem *head_src, *tail_src, *tail_dst;

    if (list_isempty(src))
        return;

    head_src = list_head(src);
    tail_src = list_tail(src);
    tail_dst = list_tail(dst);

    tail_dst->next = head_src;
    head_src->prev = tail_dst;
    tail_src->next = &dst->sentinal;
    dst->sentinal.prev = tail_src;
}

/**
 * \brief Get the first element from the list.
 *
 * \param [in] l Pointer to the linked list to get the first element from.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns a pointer to the first element of \p l.
 */
struct list_elem *list_head(const struct list *l)
{
    assert(l != NULL);

    /* Get the first valid (non-sentinal) element in the list, and return it. */
    return l->sentinal.next;
}

/**
 * \brief Get the last element from the list.
 *
 * \param [in] l Pointer to the linked list to get the last element from.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns a pointer to the last element of \p l.
 */
struct list_elem *list_tail(const struct list *l)
{
    assert(l != NULL);

    return l->sentinal.prev;
}

/**
 * \brief Get the beginning of the list for iteration.
 *
 * To iterate over a linked list, use the following snippet:
 * \code
 * struct list_elem *e;
 *
 * for (e = list_begin(l); e != list_end(l); e = list_next(e))
 * {
 *     ...
 * }
 * \endcode
 *
 * \param [in] l Pointer to the linked list to start iterating over.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns the first element in the linked list \p l.
 */
struct list_elem *list_begin(const struct list *l)
{
    assert(l != NULL);

    /* When iterating forwards, the beginning is the first valid (non-sentinal
     * element in the list. */
    return list_head(l);
}

/**
 * \brief Get the next element in the list.
 *
 * Iterate over a linked list by getting the element after \p e in the list. See
 * #list_begin() to see how to iterate over the entire list.
 *
 * \param [in] e Pointer to the current element in the list.
 *
 * \pre <tt>e != NULL</tt>
 *
 * \return Returns a pointer to the next element in the list.
 */
struct list_elem *list_next(const struct list_elem *e)
{
    assert(e != NULL);

    return e->next;
}

/**
 * \brief Get the previous element in the list.
 *
 * Iterate backwards over a linked list by getting the element before \p e in
 * the list. See #list_begin() for an example of how to iterate over a linked
 * list.
 *
 * \param [in] e Pointer to the current element in the list.
 *
 * \pre <tt>e != NULL</tt>
 *
 * \return Returns a pointer to the previous element in the list.
 */
struct list_elem *list_prev(const struct list_elem *e)
{
    assert(e != NULL);

    return e->prev;
}

/**
 * \brief Get a special end-of-list indicator.
 *
 * Gets a list element that does not correspond with any valid data. When
 * iterating over the list, this value indicates that the end of the list has
 * been reached. See #list_begin() for an example of how to iterate over the
 * list.
 *
 * \param [in] l Pointer to the linked list for which to get the end-of-list
 *          indicator.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns a pointer that indicates the end of the list has been
 *         reached.
 */
struct list_elem *list_end(const struct list *l)
{
    assert(l != NULL);

    return (struct list_elem *)&l->sentinal;
}

/**
 * \brief Push an element onto the front of a list.
 *
 * Adds the element \p e to the front of list \p l. Can be used with
 * #list_popfront() to implement a simple stack, or with #list_popback() to
 * implement a simple queue.
 *
 * \param [out] l Pointer to the list where \p e will be pushed.
 * \param [in] e Pointer to the element to add to the front of \p l.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>e != NULL</tt>
 */
void list_pushfront(struct list *l, struct list_elem *e)
{
    assert(l != NULL);
    assert(e != NULL);

    list_insert(&l->sentinal, e);
}

/**
 * \brief Push an element onto the end of a list.
 *
 * Adds the element \p e to the end of list \p l. Can be used with
 * #list_popback() to implement a simple stack, or with #list_popfront() to
 * implement a simple queue.
 *
 * \param [out] l Pointer to the list where \p e will be pushed.
 * \param [in] e Pointer to the element to add to the end of \p l.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>e != NULL</tt>
 */
void list_pushback(struct list *l, struct list_elem *e)
{
    assert(l != NULL);
    assert(l->sentinal.prev != NULL);
    assert(e != NULL);

    list_insert(l->sentinal.prev, e);
}

/**
 * \brief Determine if a list is empty.
 *
 * Constant-time operation for calculating whether or not the given list
 * contains any elements.
 *
 * \param [in] l Pointer to the list on which to determine emptyness.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns false if the list contains any elements. Returns true if
 *         there are no elements in the list.
 */
int list_isempty(const struct list *l)
{
    assert(l != NULL);

    return (l->sentinal.prev == &l->sentinal);
}

