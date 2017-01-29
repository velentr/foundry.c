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
 * linked list of said structure. Use #list_entry() to get a pointer to the
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


/**
 * \brief Initialize a linked list structure.
 *
 * Set the pointers in the given linked list structure to valid initial values.
 * This function should be called before any other functions can be used.
 *
 * \param l Pointer to the linked list to initialize.
 *
 * \pre <tt>l != NULL</tt>
 */
void list_init(struct list *l);

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
 * \param l Pointer to the linked list to start iterating over.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns the first element in the linked list \p l.
 */
struct list_elem *list_begin(struct list *l);

/**
 * \brief Get the next element in the list.
 *
 * Iterate over a linked list by getting the element after \p e in the list. See
 * #list_begin() to see how to iterate over the entire list.
 *
 * \param e Pointer to the current element in the list.
 *
 * \pre <tt>e != NULL</tt>
 *
 * \return Returns a pointer to the next element in the list.
 */
struct list_elem *list_next(struct list_elem *e);

/**
 * \brief Get the previous element in the list.
 *
 * Iterate backwards over a linked list by getting the element before \p e in
 * the list. See #list_begin() for an example of how to iterate over a linked
 * list.
 *
 * \param e Pointer to the current element in the list.
 *
 * \pre <tt>e != NULL</tt>
 *
 * \return Returns a pointer to the previous element in the list.
 */
struct list_elem *list_prev(struct list_elem *e);

/**
 * \brief Get a special end-of-list indicator.
 *
 * Gets a list element that does not correspond with any valid data. When
 * iterating over the list, this value indicates that the end of the list has
 * been reached. See #list_begin() for an example of how to iterate over the
 * list.
 *
 * \param l Pointer to the linked list for which to get the end-of-list
 *          indicator.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns a pointer that indicates the end of the list has been
 *         reached.
 */
struct list_elem *list_end(struct list *l);

/**
 * \brief Get the first element from the list.
 *
 * \param l Pointer to the linked list to get the first element from.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns a pointer to the first element of \p l.
 */
struct list_elem *list_head(struct list *l);

/**
 * \brief Get the last element from the list.
 *
 * \param l Pointer to the linked list to get the last element from.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns a pointer to the last element of \p l.
 */
struct list_elem *list_tail(struct list *l);

/**
 * \brief Insert a new element into a linked list.
 *
 * Inserts the list element \p new immediately after \p old in a linked list.
 * Note that \p old does not have to be in an existing list in order for the
 * function to work.
 *
 * \warning If \p new is already in a list \c l, then calling this function will
 * break \c l.
 *
 * \param old Pointer to a list element after which to insert \p new.
 * \param new Pointer to a list element to add to the list containing \p old.
 *
 * \pre <tt>old != NULL</tt>
 * \pre <tt>new != NULL</tt>
 */
void list_insert(struct list_elem *old, struct list_elem *new);

/**
 * \brief Push an element onto the front of a list.
 *
 * Adds the element \p e to the front of list \p l. Can be used with
 * #list_popfront() to implement a simple stack, or with #list_popback() to
 * implement a simple queue.
 *
 * \param l Pointer to the list where \p e will be pushed.
 * \param e Pointer to the element to add to the front of \p l.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>e != NULL</tt>
 */
void list_pushfront(struct list *l, struct list_elem *e);

/**
 * \brief Push an element onto the end of a list.
 *
 * Adds the element \p e to the end of list \p l. Can be used with
 * #list_popback() to implement a simple stack, or with #list_popfront() to
 * implement a simple queue.
 *
 * \param l Pointer to the list where \p e will be pushed.
 * \param e Pointer to the element to add to the end of \p l.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>e != NULL</tt>
 */
void list_pushback(struct list *l, struct list_elem *e);

/**
 * \brief Remove the given item from its containing list.
 *
 * The list will be relinked so that \p e is removed without breaking the
 * continuity of the list.
 *
 * \warning This function will exhibit undefined behavior if \p e is not already
 * in a list.
 *
 * \param e Element to remove from the list.
 *
 * \pre <tt>e != NULL</tt>
 *
 * \return Returns \p e. This is used for convenience in the internal
 *         implementation.
 */
struct list_elem *list_remove(struct list_elem *e);

/**
 * \brief Pop an element from the front of a list.
 *
 * Removes the first element from the list \p l and returns it. Can be used with
 * #list_pushfront() to implement a stack, or with #list_pushback() to implement
 * a queue.
 *
 * \warning This will exhibit undefined behavior if \p l is an empty list.
 *
 * \param l Pointer to the list from which to pop the first element.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>!list_empty(l)</tt>
 *
 * \return Returns a pointer to the element that was popped from \p l.
 */
struct list_elem *list_popfront(struct list *l);

/**
 * \brief Pop an element from the end of a list.
 *
 * Removes the last element from the list \p l and returns it. Can be used with
 * #list_pushback() to implement a stack, or with #list_pushfront() to implement
 * a queue.
 *
 * \warning This will exhibit undefined behavior if \p l is an empty list.
 *
 * \param l Pointer to the list from which to pop the last element.
 *
 * \pre <tt>l != NULL</tt>
 * \pre <tt>!list_empty(l)</tt>
 *
 * \return Returns a pointer to the element that was popped from \p l.
 */
struct list_elem *list_popback(struct list *l);

/**
 * \brief Compute the size of the linked list.
 *
 * Counts the number of elements in the list by iterating over all the elements.
 * This function will take O(n) time with respect to the length of the list. Due
 * to the sparse nature of the list, it is impractical to compute the length in
 * constant time.
 *
 * \param l Pointer to the list of which to get the size.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns the number of elements in \p l.
 */
size_t list_size(struct list *l);

/**
 * \brief Determine if a list is empty.
 *
 * Constant-time operation for calculating whether or not the given list
 * contains any elements.
 *
 * \param l Pointer to the list on which to determine emptyness.
 *
 * \pre <tt>l != NULL</tt>
 *
 * \return Returns false if the list contains any elements. Returns true if
 *         there are no elements in the list.
 */
int list_empty(struct list *l);


#endif /* end of include guard: _LIST_H_ */

