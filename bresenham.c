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
 * \file bresenham.c
 *
 * \brief Bresenham's line drawing algorithm.
 *
 * \author Brian Kubisiak
 *
 * \copyright This is free and unencumbered software released into the public
 * domain.
 */

#include <assert.h>
#include <stdlib.h>

#include "bresenham.h"

/**
 * \brief Grey code for the octants of a Cartesian plane.
 *
 * An octant refers to the slope of a given line segment. The octant of a line
 * segment is whichever octant of the Cartesian plane the line segment is in if
 * it starts at the origin. The term 'octant' throughout this code refers to
 * the octant of a line segment, not of a point.
 *
 * The actual values of each octant are a grey code to make computing the octant
 * faster. See #_getoctant() for the algorithm to compute the octant.
 *
 * Note that there is overlap between the octants on all the boundaries (i.e.
 * they are all inclusive of their bounding lines). For the purposes of
 * Bresenham's line algorithm, it does not matter which of the overlapping
 * octants is used.
 */
enum octant_t
{
    O0 = 0,
    O1 = 1,
    O2 = 5,
    O3 = 4,
    O4 = 6,
    O5 = 7,
    O6 = 3,
    O7 = 2
};

static void _switchto(enum octant_t oct, const struct point *in,
                     struct point *out);
static void _switchfrom(enum octant_t oct, const struct point *in,
                       struct point *out);
static enum octant_t _getoctant(const struct point *p0, const struct point *p1);

/**
 * \brief Draw a line between points \p p0 and \p p1, storing the output in the
 * given buffer.
 *
 * Using Bresenham's line drawing algorithm, calculates a rasterized line from
 * \p p0 to \p p1, storing the calculated pixels in the given buffer \p line. At
 * most \p len points will be stored in the buffer; any additional pixels that
 * should be drawn will be discarded. Note that \p p0 and \p p1 will both be
 * included in the calculated line.
 *
 * In order to guarantee that the entire line will be output to the \p line
 * buffer, make sure that the buffer is at least as long as the maximum of the x
 * distance and y distance between the two points.
 *
 * For more information on the algorithm, see
 * https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 *
 * \param [in] p0 Pointer to the first point in the line.
 * \param [in] p1 Pointer to the last point in the line.
 * \param [out] line Buffer in which to store the rasterized line.
 * \param [in] len Maximum number of points that may be stored in \p line.
 *
 * \return Returns the total number of points stored in \p line.
 */
size_t bresenham(const struct point *p0, const struct point *p1,
            struct point *line, size_t len)
{
    struct point np0, np1, p;
    enum octant_t oct;
    size_t i = 0;
    int dx, dy, D;

    assert(p0 != NULL);
    assert(p1 != NULL);

    /* Map the input into the 0th octant. */
    oct = _getoctant(p0, p1);
    _switchto(oct, p0, &np0);
    _switchto(oct, p1, &np1);

    /* Calculate x and y distances between the points. */
    dx = np1.x - np0.x;
    dy = np1.y - np0.y;
    D = 2*dy - dx;

    /*
     * In the 0th octant, the slope of the line segment is less than 1; thus,
     * there will be exactly one point at each x value between the two
     * endpoints.
     */
    for (p.x = np0.x, p.y = np0.y, i = 0; p.x < np1.x && i < len; p.x++, i++)
    {
        /* Map the point back to the proper octant. */
        _switchfrom(oct, &p, line + i);

        /* Change y iff the line segment rises above the given pixel. */
        if (D > 0)
        {
            p.y++;
            D -= dx;
        }

        /* Adjust the error term for the new point. */
        D += dy;
    }

    /* Add in the final point, if there is space. */
    if (i < len - 1)
    {
        _switchfrom(oct, &p, line + i);
        i++;
    }

    assert(i <= len);

    return i;
}

/**
 * \brief Map a point in the given octant into the normalized 0th octant.
 *
 * Takes the point \p in, and maps it into the 0th octant. The resulting point
 * is stored in \p out. Note that octant here does not refer to the location of
 * the point, but rather to the slope of the line segment that is being drawn.
 *
 * This function is the inverse of #_switchfrom().
 *
 * \param [in] oct The octant containing \p in.
 * \param [in] in The point to map to the 0th octant.
 * \param [out] out The resulting mapped point.
 */
static void _switchto(enum octant_t oct, const struct point *in,
                     struct point *out)
{
    assert(in != NULL);
    assert(out != NULL);

    switch (oct)
    {
    case O0:
        out->x = in->x;
        out->y = in->y;
        break;
    case O1:
        out->x = in->y;
        out->y = in->x;
        break;
    case O2:
        out->x = in->y;
        out->y = -in->x;
        break;
    case O3:
        out->x = -in->x;
        out->y = in->y;
        break;
    case O4:
        out->x = -in->x;
        out->y = -in->y;
        break;
    case O5:
        out->x = -in->y;
        out->y = -in->x;
        break;
    case O6:
        out->x = -in->y;
        out->y = in->x;
        break;
    case O7:
        out->x = in->x;
        out->y = -in->y;
        break;
    default:
        assert(0);
    }
}

/**
 * \brief Map a point in the 0th octant into the given octant.
 *
 * Takes the point \p in, located in the 0th octant, and maps it into octant \p
 * oct. The resulting point is stored in \p out. Note that octant here does not
 * refer to the location of the point, but rather to the slope of the line
 * segment that is being drawn.
 *
 * This function is the inverse of #_switchto().
 *
 * \param [in] oct The octant into which \p in is mapped.
 * \param [in] in The point to map to octant \p oct.
 * \param [out] out The resulting mapped point.
 */
static void _switchfrom(enum octant_t oct, const struct point *in,
                       struct point *out)
{
    assert(in != NULL);
    assert(out != NULL);

    switch (oct)
    {
    case O0:
        out->x = in->x;
        out->y = in->y;
        break;
    case O1:
        out->x = in->y;
        out->y = in->x;
        break;
    case O2:
        out->x = -in->y;
        out->y = in->x;
        break;
    case O3:
        out->x = -in->x;
        out->y = in->y;
        break;
    case O4:
        out->x = -in->x;
        out->y = -in->y;
        break;
    case O5:
        out->x = -in->y;
        out->y = -in->x;
        break;
    case O6:
        out->x = in->y;
        out->y = -in->x;
        break;
    case O7:
        out->x = in->x;
        out->y = -in->y;
        break;
    default:
        assert(0);
    }
}

/**
 * \brief Get the octant for the given line segment.
 *
 * Calculates the slope of the line segment from \p p0 to \p p1, and converts
 * this into a grey code representation of the octant containing the line
 * segment.
 *
 * \param [in] p0 Start point of the line segment.
 * \param [in] p1 End point of the line segment.
 *
 * \return Returns the octant containing the line segment from \p p0 to \p p1.
 */
static enum octant_t _getoctant(const struct point *p0, const struct point *p1)
{
    int dx, dy;

    assert(p0 != NULL);
    assert(p1 != NULL);

    /* Get the deltas for x and y for determining the octant. */
    dx = p1->x - p0->x;
    dy = p1->y - p0->y;

    /* Convert dx and dy into a grey code representing the octant. */
    return (enum octant_t)((dx >  0                 ? 0 : 4)
                         | (dy >  0                 ? 0 : 2)
                         | (dx != 0 && dy / dx == 0 ? 0 : 1));
}

