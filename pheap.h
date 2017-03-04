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
 * \file pheap.h
 *
 * \brief Arbitrarily large pairing heaps.
 */

#ifndef _PHEAP_H_
#define _PHEAP_H_


#include "list.h"
#include "utils.h"

/**
 * \brief Node in a pairing heap.
 *
 * This node element can be embedded into another structure in order to add the
 * structure to a pairing heap. Use #containerof() to get a pointer to the
 * containing structure.
 *
 * Must be initialized with #pheap_elem_init() before it can be pushed onto a
 * heap.
 */
struct pheap_elem
{
    struct list children;      /**< List of children of this node. */
    struct list_elem child_le; /**< List element to add this node to its
                                    parent's list. */
};

/**
 * \brief Pairing heap structure.
 *
 * The heap contains a root node, which is the minimum of the heap, as well as a
 * compare function for comparing two elements on the heap. The root node will,
 * in turn, contain a list of its children, creating a recursive structure. Note
 * that this structure is used as a container for binding the compare function
 * to each node in the heap.
 *
 * Must be initialized with #pheap_init() before it can be used.
 */
struct pheap
{
    struct pheap_elem *root; /**< Root (minimum) element of the heap. */
    cmp_func cmp;            /**< Function for comparing two elements. */
};

void pheap_elem_init(struct pheap_elem *pe);
void pheap_init(struct pheap *ph, cmp_func cmp);
struct pheap_elem *pheap_peek(const struct pheap *ph);
void pheap_push(struct pheap *ph, struct pheap_elem *pe);
void pheap_pop(struct pheap *ph);
int pheap_isempty(struct pheap *ph);


#endif /* end of include guard: _PHEAP_H_ */

