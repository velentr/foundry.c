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
 * \file fixpt.c
 *
 * \brief Fixed-point arithmetic.
 */

#include <assert.h>
#include <stdint.h>

#include "fixpt.h"

fixpt_t fixpt_add(fixpt_t a, fixpt_t b)
{
    /* Check for overflow */
    assert((int64_t)a + (int64_t)b < ((int64_t)1 << FIXPT_TOTAL));

    return a + b;
}

fixpt_t fixpt_sub(fixpt_t a, fixpt_t b)
{
    /* Check for usigned underflow */
    assert(b <= a);
    return a - b;
}

fixpt_t fixpt_mul(fixpt_t a, fixpt_t b)
{
    int64_t prod;

    prod = (int64_t)a * (int64_t)b;

    /* Check for overflow */
    assert((prod >> FIXPT_FRAC)
                    < ((int64_t)1 << FIXPT_TOTAL));

    return prod >> FIXPT_FRAC;
}

fixpt_t fixpt_div(fixpt_t a, fixpt_t b)
{
    int64_t div;

    div = (int64_t)a << FIXPT_FRAC;

    return (fixpt_t)(div / b);
}

fixpt_t fixpt_from_int(int32_t n)
{
    assert((n >> FIXPT_INT) == 0);
    return n << FIXPT_FRAC;
}

