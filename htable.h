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
 * \file htable.h
 *
 * \author Brian Kubisiak
 *
 * \brief Hash table.
 *
 * Implementation for a hash table with no internal memory allocation.
 * All memory allocation is performed by adding #hash_elem structures to
 * existing datatypes; this allows all memory allocation to be completely
 * external.
 *
 * The hashing function is user-provided and will likely require getting the
 * container class from the #hash_elem that is passed as an argument.
 */

#ifndef _HTABLE_H_
#define _HTABLE_H_


#include "list.h"
#include "utils.h"

/**
 * \brief Element stored in a hash table.
 */
struct hash_elem
{
    struct list_elem le; /**< List element for storing within a bucket. */
};

/**
 * \brief Function for hashing an element into the hash table.
 *
 * This function takes a hash element and the size of the hash table and returns
 * a hash value indicating which bucket to use for storing the element. This
 * function will likely act on the container of \p e rather than \p e itself.
 *
 * A good hash function should return a value that is deterministic, but still
 * appears to be random with respect to the data that is being hashed. Small
 * changes in the hashed data should result in large changes in the hash value.
 *
 * \param e Hash element that should be hashed.
 * \param numbuckets Total number of buckets in the hash table.
 *
 * \post Return value is smaller than \p numbuckets.
 *
 * \return Returns a hash value that will be used to index into the array of
 * buckets. The returned value should be smaller than \p numbuckets.
 */
typedef size_t (*hasher)(const struct hash_elem *e, size_t numbuckets);

/**
 * \brief Hash table for associative arrays of objects.
 *
 * The hash table is implemented as an array of linked lists. Each slot in the
 * array holds a linked list of elements that have the same hash value. Hash
 * values are determined with the user-provided \c hash function. The \c cmp
 * function is used to find hash collisions.
 *
 * Lookup time (assuming minimal collisions) is in constant time. Hash elements
 * should be embedded embedded in container structures.
 */
struct hash_table
{
    struct list *buckets; /**< Memory for holding the buckets of the table. */
    hasher hash;          /**< Function for hashing elements. */
    cmp_func cmp;         /**< Function for comparing elements. Passed pointers
                            to \c hash_elems as arguments. */
    size_t len;           /**< Length of the \c buckets array. */
};

void ht_init(struct hash_table *ht, struct list *buckets, size_t num,
        hasher hash, cmp_func cmp);
void ht_insert(struct hash_table *ht, struct hash_elem *he);
struct hash_elem *ht_get(const struct hash_table *ht,
        const struct hash_elem *key);
struct list *ht_rehash(struct hash_table *ht, struct list *buckets, size_t num);
size_t ht_size(const struct hash_table *ht);
int ht_isempty(const struct hash_table *ht);
struct hash_elem *ht_remove(struct hash_elem *he);
size_t ht_space(const struct hash_table *ht);


#endif /* end of include guard: _HTABLE_H_ */

