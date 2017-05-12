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
 * \file fixpt.h
 *
 * \brief Fixed-point arithmetic.
 */

#ifndef _FIXPT_H_
#define _FIXPT_H_


#include <stdint.h>

#ifndef FIXPT_TOTAL
/**
 * \brief Total number of bits in the fixed-point type.
 */
#define FIXPT_TOTAL 32
#endif

#ifndef FIXPT_FRAC
/**
 * \brief Number of fractional bits in the fixed-point type.
 */
#define FIXPT_FRAC 16
#endif

/**
 * \brief Number of integer bits in the fixed-point type.
 */
#define FIXPT_INT (FIXPT_TOTAL - FIXPT_FRAC - 1)

#if FIXPT_INT < 0
#error Too many fractional bits
#endif

/**
 * \brief Fixed-point type.
 */
typedef int32_t fixpt_t;

fixpt_t fixpt_add(fixpt_t a, fixpt_t b);
fixpt_t fixpt_sub(fixpt_t a, fixpt_t b);
fixpt_t fixpt_mul(fixpt_t a, fixpt_t b);
fixpt_t fixpt_div(fixpt_t a, fixpt_t b);
fixpt_t fixpt_from_int(int32_t n);
int32_t fixpt_to_int(fixpt_t n);


#endif /* end of include guard: _FIXPT_H_ */

