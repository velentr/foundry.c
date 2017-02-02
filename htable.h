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
typedef size_t (*hasher)(struct hash_elem *e, size_t numbuckets);

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

/**
 * \brief Initialize a hash table structure.
 *
 * Gives a block of memory to the hash table to use for the buckets, and
 * provides hashing and comparison functions for storing the elements.
 *
 * \param ht Pointer to the hash table to initialize.
 * \param buckets Memory block to store the buckets of the hash table.
 * \param num Number of buckets that can be stored in \p buckets.
 * \param hash Function for computing the hash of an element.
 * \param cmp Function for detecting whether two hash elements are equal.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>buckets != NULL</tt>
 * \pre <tt>num > 0</tt>
 * \pre <tt>hash != NULL</tt>
 * \pre <tt>cmp != NULL</tt>
 */
void ht_init(struct hash_table *ht, struct list *buckets, size_t num,
        hasher hash, cmp_func cmp);

/**
 * \brief Insert a new element into the hash table.
 *
 * Assumes that an equivalent element is not already in the hash table. In order
 * to check this, the user should first check that #ht_get() returns a \c NULL
 * pointer. Also assumes that the given \p he is not already in a hash table.
 *
 * \param ht Pointer to the hash table in which to insert the element.
 * \param he Pointer to the new element to insert.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>he != NULL</tt>
 * \pre <tt>ht_get(ht, he) == NULL</tt>
 *
 * \note Calling #ht_insert() on an element that is already contained in a hash
 * table will break the hash table that it is already in.
 */
void ht_insert(struct hash_table *ht, struct hash_elem *he);

/**
 * \brief Remove an element from its containing hash table.
 *
 * Given a pointer to an element already in a hash table, removes the element
 * from its container. In order to get the element to remove, use the #ht_get()
 * function.
 *
 * \param he Pointer to the hash element to remove.
 *
 * \pre <tt>he != NULL</tt>
 *
 * \return Returns \p he, as a convenience.
 */
struct hash_elem *ht_remove(struct hash_elem *he);

/**
 * \brief Finds the hash element matching \p key from the given hash table.
 *
 * Searches through \p ht until an element \c he is found such that
 * <tt>ht->cmp(key, he) == 0</tt>. If no such element exists in the hash table,
 * then \c NULL is returned.
 *
 * \param ht Pointer to hash table to search for the element matching \p key.
 * \param key Pointer to a hash element to use as a key for the search.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>key != NULL</tt>
 *
 * \return Returns a pointer to an element in the table with a matching key. If
 * no such element is found, returns \c NULL.
 */
struct hash_elem *ht_get(struct hash_table *ht, struct hash_elem *key);

/**
 * \brief Give the hash table a different block of memory to use.
 *
 * Tells the hash table to use the array \p buckets for storing all of its
 * elements. This will usually be used to increase the memory pool when the
 * number of collisions gets too high.
 *
 * \param ht Pointer to the hash table that should use \p buckets.
 * \param buckets Array of lists to use for storing buckets in the hash table.
 * \param num The length of the \p buckets array.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>buckets != NULL</tt>
 * \pre <tt>num > 0</tt>
 *
 * \return Returns a pointer to the old array used to store the buckets. This
 * array may be freed after the call if it is no longer needed.
 */
struct list *ht_rehash(struct hash_table *ht, struct list *buckets, size_t num);

/**
 * \brief Get the number of elements stored in the hash table.
 *
 * Due to the way that some of the hash operations work, this function must
 * iterate over the entire table to count its contents; this can get pretty
 * slow, so it may be better to record this information elsewhere.
 *
 * \param ht Pointer to the hash table to count the elements of.
 *
 * \pre <tt>ht != NULL</tt>
 *
 * \return Returns the number of elements currently stored in the hash table.
 */
size_t ht_size(struct hash_table *ht);

/**
 * \brief Get the total number of buckets in the hash table.
 *
 * This can be used to calculate whether or not the hash table needs to be
 * rehashed.
 *
 * \param ht Pointer to the hash table to count the buckets of.
 *
 * \pre <tt>ht != NULL</tt>
 *
 * \return Returns the total number of buckets currently stored by the hash
 * table.
 */
size_t ht_space(struct hash_table *ht);

/**
 * \brief Determine whether or not a hash table is empty.
 *
 * \param ht Pointer to the hash table to check for emptiness.
 *
 * \pre <tt>ht != NULL</tt>
 *
 * \return Returns zero if the hash table contains any elements. Returns nonzero
 * if the hash table is empty.
 */
int ht_isempty(struct hash_table *ht);


#endif /* end of include guard: _HTABLE_H_ */

