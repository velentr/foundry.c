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
 * \file blkalloc.h
 *
 * \brief Memory allocator for fixed-size blocks.
 */

#ifndef _BLKALLOC_H_
#define _BLKALLOC_H_


#include <stdint.h>
#include <stdlib.h>

#include "list.h"

/**
 * Structure containing the state and memory pools of a fixed-size block
 * allocator.
 */
struct blkalloc
{
    struct list pools;      /**< List of memory pools for allocating blocks. */
    void *(*alloc)(size_t); /**< Underlying allocator used for getting pools. */
    void (*free)(void *);   /**< Underlying free for freeing pools. */
    uintptr_t *freeblks;    /**< Explicit free list of free blocks. */
    size_t pool_size;       /**< Number of blocks per pool. */
    size_t blk_size;        /**< Size of each block. */
};

int blkalloc_init(struct blkalloc *a, void *(*alloc)(size_t),
        void (*free)(void *), size_t blk_size, size_t pool_size);
void blkalloc_destroy(struct blkalloc *a);
void *blkalloc_alloc(struct blkalloc *a);
void blkalloc_free(struct blkalloc *a, void *_blk);


#endif /* end of include guard: _BLKALLOC_H_ */

