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
 * \file htable.c
 *
 * \brief Hash table using linked lists for hash collisions.
 */

#include <assert.h>
#include <stdlib.h>

#include "htable.h"
#include "list.h"
#include "utils.h"

/**
 * \brief Initialize a hash table structure.
 *
 * Gives a block of memory to the hash table to use for the buckets, and
 * provides hashing and comparison functions for storing the elements.
 *
 * \param [in,out] ht Pointer to the hash table to initialize.
 * \param [in] buckets Memory block to store the buckets of the hash table.
 * \param [in] num Number of buckets that can be stored in \p buckets.
 * \param [in] hash Function for computing the hash of an element.
 * \param [in] cmp Function for detecting whether two hash elements are equal.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>buckets != NULL</tt>
 * \pre <tt>num > 0</tt>
 * \pre <tt>hash != NULL</tt>
 * \pre <tt>cmp != NULL</tt>
 */
void ht_init(struct hash_table *ht, struct list *buckets, size_t num,
        hasher hash, cmp_func cmp)
{
    size_t i;

    assert(ht != NULL);
    assert(buckets != NULL);
    assert(num > 0);
    assert(hash != NULL);
    assert(cmp != NULL);

    /* Initialize all members of the hash table. */
    ht->buckets = buckets;
    ht->hash = hash;
    ht->cmp = cmp;
    ht->len = num;

    /* Initialize all the buckets. */
    for (i = 0; i < num; i++)
    {
        list_init(buckets + i);
    }
}

/**
 * \brief Insert a new element into the hash table.
 *
 * Assumes that an equivalent element is not already in the hash table. In order
 * to check this, the user should first check that #ht_get() returns a \c NULL
 * pointer. Also assumes that the given \p he is not already in a hash table.
 *
 * \param [out] ht Pointer to the hash table in which to insert the element.
 * \param [in] he Pointer to the new element to insert.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>he != NULL</tt>
 * \pre <tt>ht_get(ht, he) == NULL</tt>
 *
 * \note Calling #ht_insert() on an element that is already contained in a hash
 * table will break the hash table that it is already in.
 */
void ht_insert(struct hash_table *ht, struct hash_elem *he)
{
    /* Hash value for the given key. */
    size_t hval;

    assert(ht != NULL);
    assert(he != NULL);
    assert(ht_get(ht, he) == NULL);

    /* Get the hash value for the element. */
    hval = ht->hash(he, ht->len);

    assert(hval < ht->len);

    /* Add the element to the appropriate hash bucket. */
    list_pushfront(&ht->buckets[hval], &he->le);
}

/**
 * \brief Finds the hash element matching \p key from the given hash table.
 *
 * Searches through \p ht until an element \c he is found such that
 * <tt>ht->cmp(key, he) == 0</tt>. If no such element exists in the hash table,
 * then \c NULL is returned.
 *
 * \param [in] ht Pointer to hash table to search for the element matching \p
 * key.
 * \param [in] key Pointer to a hash element to use as a key for the search.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>key != NULL</tt>
 *
 * \return Returns a pointer to an element in the table with a matching key. If
 * no such element is found, returns \c NULL.
 */
struct hash_elem *ht_get(const struct hash_table *ht,
        const struct hash_elem *key)
{
    size_t hval;           /* Hash value for the given key. */
    struct list *bucket;   /* Bucket to search through for key. */
    struct hash_elem *rc;  /* Hash element to return. */
    struct list_elem *le;  /* Iterator for the bucket containing key. */
    cmp_func cmp;          /* Comparator function. */

    assert(ht != NULL);
    assert(key != NULL);

    /* Get the bucket to search through. */
    hval = ht->hash(key, ht->len);
    assert(hval < ht->len);
    bucket = &ht->buckets[hval];
    cmp = ht->cmp;

    /* Return NULL if the key isn't found. */
    rc = NULL;

    /* Iterate over the bucket, searching for key. */
    for (le = list_begin(bucket); le != list_end(bucket); le = list_next(le))
    {
        /* Get the hash entry corresponding to the current list iterator. */
        struct hash_elem *val = containerof(le, struct hash_elem, le);

        /* If the value matches the key, then return it. */
        if (cmp(val, key) == 0)
        {
            rc = val;
            break;
        }
    }

    return rc;
}

/**
 * \brief Give the hash table a different block of memory to use.
 *
 * Tells the hash table to use the array \p buckets for storing all of its
 * elements. This will usually be used to increase the memory pool when the
 * number of collisions gets too high.
 *
 * \param [out] ht Pointer to the hash table that should use \p buckets.
 * \param [in] buckets Array of lists to use for storing buckets in the hash
 * table.
 * \param [in] num The length of the \p buckets array.
 *
 * \pre <tt>ht != NULL</tt>
 * \pre <tt>buckets != NULL</tt>
 * \pre <tt>num > 0</tt>
 *
 * \return Returns a pointer to the old array used to store the buckets. This
 * array may be freed after the call if it is no longer needed.
 */
struct list *ht_rehash(struct hash_table *ht, struct list *buckets, size_t num)
{
    struct list *old_buckets;   /* Old memory pool for the buckets. */
    size_t old_num;             /* Old number of buckets in the hash table. */
    size_t i;                   /* Iterator over the old buckets. */

    assert(ht != NULL);
    assert(buckets != NULL);
    assert(num > 0);

    /* Store the new buckets array in the hash table. */
    old_buckets = ht->buckets;
    old_num = ht->len;
    ht_init(ht, buckets, num, ht->hash, ht->cmp);

    /* Iterate over all of the old buckets. */
    for (i = 0; i < old_num; i++)
    {
        struct list_elem *le;
        struct list *b = old_buckets + i;

        /* Iterate over all the elements in each of the old buckets. */
        for (le = list_begin(b); le != list_end(b); le = list_next(le))
        {
            /* For each hash element, remove it from the old bucket and add it
             * to a new bucket. */
            struct hash_elem *val = containerof(le, struct hash_elem, le);
            (void)ht_remove(val);
            ht_insert(ht, val);
        }
    }

    /* This array is no longer used by the hash table. */
    return old_buckets;
}

/**
 * \brief Get the number of elements stored in the hash table.
 *
 * Due to the way that some of the hash operations work, this function must
 * iterate over the entire table to count its contents; this can get pretty
 * slow, so it may be better to record this information elsewhere.
 *
 * \param [in] ht Pointer to the hash table to count the elements of.
 *
 * \pre <tt>ht != NULL</tt>
 *
 * \return Returns the number of elements currently stored in the hash table.
 */
size_t ht_size(const struct hash_table *ht)
{
    size_t nelems;          /* Counter of the number of elements in ht. */
    size_t len;             /* Length of the array of buckets. */
    struct list *buckets;   /* Array of buckets to search. */
    size_t i;               /* Iterator over the buckets. */

    assert(ht != NULL);

    nelems = 0;
    len = ht->len;
    buckets = ht->buckets;

    /* Iterate over each of the buckets. */
    for (i = 0; i < len; i++)
    {
        /* Add the length of the bucket to the size of the hash table. */
        nelems += list_size(buckets + i);
    }

    return nelems;
}

/**
 * \brief Determine whether or not a hash table is empty.
 *
 * \param [in] ht Pointer to the hash table to check for emptiness.
 *
 * \pre <tt>ht != NULL</tt>
 *
 * \return Returns zero if the hash table contains any elements. Returns nonzero
 * if the hash table is empty.
 */
int ht_isempty(const struct hash_table *ht)
{
    size_t i;
    size_t len;
    struct list *buckets;

    assert(ht != NULL);

    buckets = ht->buckets;
    len = ht->len;

    /* Loop through each buckets, checking whether or not it is empty. */
    for (i = 0; i < len; i++)
    {
        if (!list_isempty(buckets + i))
        {
            return 0;
        }
    }

    /* If all the buckets are empty, then the hash table is empty. */
    return 1;
}

