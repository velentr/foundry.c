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
 * For more information, please refer to <http://unlicense.org/>
 */

/**
 * \file bresenham.h
 *
 * \brief Bresenham's line drawing algorithm.
 *
 * To draw a line between the 2d cartesian points \c p0 and \c p1, call the
 * function #bresenham() with \c p0 and \c p1 as arguments, along with a buffer
 * for storing the points in the resulting line.
 *
 * \author Brian Kubisiak
 *
 * \copyright This is free and unencumbered software released into the public
 * domain.
 */

#ifndef _BRESENHAM_H_
#define _BRESENHAM_H_


#include <stdlib.h>

/**
 * \brief A single point in 2d space, using Cartesian coordinates.
 */
struct point
{
    int x;  /**< x-coordinate of the point. */
    int y;  /**< y-coordinate of the point. */
};

size_t bresenham(const struct point *p0, const struct point *p1,
            struct point *line, size_t len);


#endif /* end of include guard:  */

