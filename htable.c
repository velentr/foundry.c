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

/* Initialize all the members in the hash table. */
void ht_init(struct hash_table *ht, struct list *buckets, size_t num,
        hasher hash, hcompar cmp)
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

/* Insert an element into the hash table. */
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

/* Remove an existing hash element from its containing hash table. */
struct hash_elem *ht_remove(struct hash_elem *he)
{
    assert(he != NULL);

    (void)list_remove(&he->le);

    return he;
}

/* Get an element from the given hash table that is equal to the given key. */
struct hash_elem *ht_get(struct hash_table *ht, struct hash_elem *key)
{
    size_t hval;           /* Hash value for the given key. */
    struct list *bucket;   /* Bucket to search through for key. */
    struct hash_elem *rc;  /* Hash element to return. */
    struct list_elem *le;  /* Iterator for the bucket containing key. */
    hcompar cmp;           /* Comparator function. */

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

/* Use a different block of memory for the hash table. */
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

/* Get the number of elements stored in the given hash table. */
size_t ht_size(struct hash_table *ht)
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

/* Get the number of buckets in the hash table. */
size_t ht_space(struct hash_table *ht)
{
    assert(ht != NULL);

    return ht->len;
}

/* See if the given hash table is empty. */
int ht_isempty(struct hash_table *ht)
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

