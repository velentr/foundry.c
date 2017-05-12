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

/**
 * \brief Add two fixed-point numbers.
 *
 * Returns the fixed-point value \f$n = a + b\f$.
 *
 * \param [in] a The augend for the operation.
 * \param [in] b The addend for the operation.
 *
 * \return Returns the sum of \p a and \p b.
 *
 * \note With assertions enabled, this function will abort on overflow. Compile
 * with \c NDEBUG defined in order to disable.
 */
fixpt_t fixpt_add(fixpt_t a, fixpt_t b)
{
    /* Check for overflow */
    assert((int64_t)a + (int64_t)b < ((int64_t)1 << FIXPT_TOTAL));

    return a + b;
}

/**
 * \brief Subtract two fixed-point numbers.
 *
 * Returns the fixed-point value \f$n = a - b\f$.
 *
 * \param [in] a The minuend of the operation.
 * \param [in] b The subtractend of the operation.
 *
 * \return Returns the difference of \p a and \p b.
 */
fixpt_t fixpt_sub(fixpt_t a, fixpt_t b)
{
    return a - b;
}

/**
 * \brief Multiply two fixed-point numbers.
 *
 * Returns the fixed-point value \f$n = a \times b\f$.
 *
 * \param [in] a The multiplicand of the operation.
 * \param [in] b The multiplier of the operation.
 *
 * \return Returns the product of \p a and \p b.
 *
 * \note With assertions enabled, this function will abort on overflow. Compile
 * with \c NDEBUG defined in order to disable.
 */
fixpt_t fixpt_mul(fixpt_t a, fixpt_t b)
{
    int64_t prod;

    prod = (int64_t)a * (int64_t)b;

    /* Check for overflow */
    assert((prod >> FIXPT_FRAC)
                    < ((int64_t)1 << FIXPT_TOTAL));

    return prod >> FIXPT_FRAC;
}

/**
 * \brief Divide two fixed-point numbers.
 *
 * Returns the fixed-point value \f$n = a / b\f$.
 *
 * \param [in] a The dividend of the operation.
 * \param [in] b The divisor of the operation.
 *
 * \return Returns the quotient of \p a and \p b.
 */
fixpt_t fixpt_div(fixpt_t a, fixpt_t b)
{
    int64_t div;

    div = (int64_t)a << FIXPT_FRAC;

    return (fixpt_t)(div / b);
}

/**
 * \brief Convert an integer to a fixed-point value.
 *
 * \param [in] n The integer to convert.
 *
 * \return Returns the fixed-point representation of \p n.
 *
 * \note With assertions enabled, this function will abort on overflow. Compile
 * with \c NDEBUG defined in order to disable.
 */
fixpt_t fixpt_from_int(int32_t n)
{
    assert((n >> FIXPT_INT) == 0);
    return n << FIXPT_FRAC;
}

/**
 * \brief Convert a fixed-point value to an integer.
 *
 * If \p n is not an integer, then it is truncated.
 *
 * \param [in] n The fixed-point value to convert.
 *
 * \return Returns the integer portion of \p n.
 */
int32_t fixpt_to_int(fixpt_t n)
{
    return n >> FIXPT_FRAC;
}

