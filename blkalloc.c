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
 * \file blkalloc.c
 *
 * \brief Implementation of a fixed-size block allocator.
 *
 * A block is a single fixed-size chunk of data that is requested by the user
 * using #blkalloc_alloc(). Each block allocator can handle a single block
 * size; if the user needs multiple different block sizes, he should use
 * multiple block allocators.
 *
 * A pool is an internal-only fixed-size array of blocks. The block allocator
 * uses pools internally to bulk allocate/free blocks of data.
 *
 * Internally, the block allocator is laid out as a doubly-linked list of pools
 * and a singly-linked explicit free list of blocks. Once the free list is
 * empty, a new pool is allocated and divided into a new free list. Allocating a
 * new block is simply popping a block off the front of the free list. Freeing a
 * block is simply pushing it onto the front of the free list.
 *
 * Note that lazy allocation is used for getting pools; no memory is allocated
 * during initialization, so the first call to #blkalloc_alloc() will take extra
 * time to allocate a pool before returning a new block.
 *
 * \todo Add safety checks to make sure freed blocks are actually blocks from
 * this allocator.
 * \todo Add valgrind instrumentation for debugging.
 */

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#include <valgrind/memcheck.h>
#include <valgrind/valgrind.h>

#include "blkalloc.h"
#include "list.h"
#include "utils.h"

/**
 * Fixed-size pool of blocks for allocation.
 */
struct blkpool
{
    struct list_elem le;    /**< Entry on the list of pools. */
    uintptr_t blks[];       /**< Array of blocks for allocation. */
};

static uintptr_t *_blkpool_init(struct blkpool *p, size_t blk_size,
        size_t pool_size);

/**
 * \brief Initialize a block allocator.
 *
 * Initialize the block allocator for allocating blocks of size \p blk_size. Use
 * \p f_alloc and \p f_free for the underlying memory management. If an invalid
 * parameter is detected, returns -1. If successful, returns 0.
 *
 * If -1 is returned, then the allocator is not initialized, and
 * #blkalloc_destroy() should not be called for the allocator.
 *
 * After the allocator has been initialized, it is ready to allocate blocks with
 * #blkalloc_alloc(). Note that since a lazy initialization is used, the first
 * block allocated will take extra time, since a new pool must be allocated
 * using the underlying memory management routines. Subsequent blocks will be
 * allocated more quickly, until a second pool is needed.
 *
 * \param [in] a Block allocator to initialize.
 * \param [in] f_alloc Underlying allocation function. The function should
 * behave the same as \c malloc(3).
 * \param [in] f_free Underlying free function. The function should behave the
 * same as \c free(3).
 * \param [in] blk_size Size (in bytes) of each allocated block. Must be a
 * multiple of the system's pointer size in order to keep blocks properly
 * aligned.
 * \param [in] pool_size Number of blocks to allocate in each pool. Making this
 * larger will cause the allocator to user more memory, but may result in faster
 * allocations.
 *
 * \return If successful, returns 0. If an invalid parameter is detected,
 * returns -1. An invalid parameter could be a \p blk_size that is too small, a
 * \p blk_size that is not a multiple of the pointer size, \p blk_size or \p
 * pool_size of zero, or too large \p blk_size or \p pool_size such that the
 * size of the resulting pool would overflow an unsigned integer.
 */
int blkalloc_init(struct blkalloc *a, void *(*f_alloc)(size_t),
        void (*f_free)(void *), size_t blk_size, size_t pool_size)
{
    /*
     * Check for invalid parameters:
     *  - Each block must be large enough to hold a pointer for bookkeeping.
     *  - Each block must be aligned on sizeof(uintptr_t) boundaries, to aid in
     *    portability. Speed is a welcome side effect.
     *  - Each pool must contain a nonzero number of blocks (for obvious
     *    reasons).
     *  - Make sure that there is no overflow in the multiplication/addition for
     *    calculating the pool size.
     */
    if (blk_size < sizeof(uintptr_t)
     || blk_size % sizeof(uintptr_t) != 0
     || pool_size == 0
     || (UINT_MAX - sizeof(struct blkpool)) / pool_size < blk_size)
    {
        return -1;
    }

    /* Register the new block allocator with valgrind. */
    VALGRIND_CREATE_MEMPOOL(a, 0, 0);

    /* If parameter checks pass, initialize the allocator. */
    list_init(&a->pools);
    a->alloc = f_alloc;
    a->free = f_free;
    a->freeblks = NULL;
    a->pool_size = pool_size;
    a->blk_size = blk_size;

    return 0;
}

/**
 * \brief Destroy a block allocator.
 *
 * Deallocates all the pools used by the block allocator \p a. It is not
 * necessary to individually free all the blocks that were previously allocated
 * using \p a before calling this function, but after a block allocator is
 * destroyed, any blocks allocated using it should not be used.
 *
 * \param [in] a Block allocator to destroy.
 */
void blkalloc_destroy(struct blkalloc *a)
{
    struct blkpool *p;

    /* Pop each pool out of the list of pools and bulk-free it. */
    while (!list_isempty(&a->pools))
    {
        p = containerof(list_popback(&a->pools), struct blkpool, le);

        /* Bulk-free all blocks in the pool for valgrind. */
        VALGRIND_MEMPOOL_TRIM(a, p->blks, a->blk_size * a->pool_size);

        a->free(p);
    }

    /* Unregister the block allocator with valgrind. */
    VALGRIND_DESTROY_MEMPOOL(a);
}

/**
 * \brief Allocate a block.
 *
 * Using the given block allocator \p a, allocate a new block in memory and
 * return it. If there are no blocks available in any memory pools, then a new
 * pool is allocated.
 *
 * Runs in O(1) time.
 *
 * \param [in] a Block allocator from which a new block is allocated.
 *
 * \return Returns a pointer to the new block, or \c NULL if no new block could
 * be allocated.
 */
void *blkalloc_alloc(struct blkalloc *a)
{
    struct blkpool *newpool;
    uintptr_t *ret;

    /* Use the first block in the free list as the new block. */
    ret = a->freeblks;

    /* No more blocks available in any of the pools. */
    if (ret == NULL)
    {
        /* Allocate memory for a new pool. */
        newpool = a->alloc(sizeof(struct blkpool) + a->pool_size * a->blk_size);
        if (newpool == NULL)
        {
            return NULL;
        }

        /* Add the new pool to the list of block pools. */
        list_pushback(&a->pools, &newpool->le);

        /* Initialize the pool; use the first free value as the new block. */
        ret = a->freeblks = _blkpool_init(newpool, a->blk_size, a->pool_size);

        /* Mark the superblock of the new pool as inaccessible. */
        VALGRIND_MAKE_MEM_NOACCESS(newpool->blks, a->pool_size * a->blk_size);
    }

    /* Temporarily treat the new block as defined so we can read bookkeeping
     * data from it without getting errors from valgrind. */
    VALGRIND_MAKE_MEM_DEFINED(a->freeblks, a->blk_size);

    /* Remove the new block from the free list. */
    a->freeblks = (uintptr_t *)(*a->freeblks);

    /* Record the allocated block with valgrind. */
    VALGRIND_MEMPOOL_ALLOC(a, ret, a->blk_size);

    return ret;
}

/**
 * \brief Free a block.
 *
 * Frees the given \p _blk and adds it back to the explicit free list. \p _blk
 * should be a block previously allocated using #blkalloc_alloc() from the same
 * block allocator \p a.
 *
 * This function runs in O(1) time.
 *
 * \param [in] a Block allocator from which \p _blk was allocated.
 * \param [in] _blk Previously allocated block that should be freed.
 */
void blkalloc_free(struct blkalloc *a, void *_blk)
{
    uintptr_t *blk = _blk;

    /* Add the freed block to the head of the free list. */
    *blk = (uintptr_t)a->freeblks;
    a->freeblks = blk;

    /* Register the newly-freed block in valgrind. */
    VALGRIND_MEMPOOL_FREE(a, _blk);
}

/**
 * \brief Initialize a newly allocated pool.
 *
 * Assuming \p p is a pool allocated with \p pool_size blocks of size \p
 * blk_size, generate an explicit free list containing all of the blocks.
 * Returns a pointer to the head of the free list.
 *
 * \param [inout] p Pool to initialize.
 * \param [in] blk_size Size of each block in the pool.
 * \param [in] pool_size Number of blocks in the pool.
 *
 * \return Returns the start of the explicit free list in the pool.
 */
static uintptr_t *_blkpool_init(struct blkpool *p, size_t blk_size, size_t pool_size)
{
    size_t inc;
    size_t i;
    uintptr_t *blk;

    /* Each block must be able to fit a pointer (for bookkeeping). */
    assert(blk_size >= sizeof(uintptr_t));
    /* Blocks must be aligned on pointer boundaries. */
    assert(blk_size % sizeof(uintptr_t) == 0);

    inc = blk_size / sizeof(uintptr_t);
    blk = p->blks;

    /* Chain all the blocks together in a free list. */
    for (i = 0; i < pool_size - 1; i++)
    {
        *blk = (uintptr_t)(blk + inc);
        blk += inc;
    }

    /* Last block stores a NULL pointer. */
    *blk = 0;

    return p->blks;
}

