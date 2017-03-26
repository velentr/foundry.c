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
 * \file utils.h
 *
 * \brief Miscellaneous utilities.
 *
 * This file contains all the random functions and macros that I use pretty
 * often.
 *
 * \author Brian Kubisiak
 */

#ifndef _UTILS_H_
#define _UTILS_H_


#include <stddef.h>

/**
 * \brief Get a pointer to the structure containing the given element.
 *
 * If you have a structure \c str that contains an element in member \c e, and
 * you have a pointer to an element \c elem, you can get a pointer to the
 * containing structure \c str with the code
 * \code
 * containerof(elem, str, e);
 * \endcode
 *
 * \param ELEM Pointer to the element of interest.
 * \param STRUCT Name of the structure containing the element.
 * \param MEMBER Member of \p STRUCT that holds the element.
 *
 * \return Returns a pointer to the structure containing \p ELEM.
 */
#define containerof(ELEM, STRUCT, MEMBER) ((STRUCT *)((size_t) (ELEM) \
            - offsetof(STRUCT, MEMBER)))

/**
 * \brief Get the length of a stack-allocated array.
 *
 * Given a stack-allocated array (e.g. int arr[4]), returns the size of the
 * total array divided by the size of each individual element. If the argument
 * is not a stack-allocated array, then behavior is undefined; the code may
 * still compile and run, but you will not get the result you are looking for.
 *
 * \param ARR The array of which to find the length.
 *
 * \return Returns the length of the given array \p ARR.
 */
#define lengthof(ARR) (sizeof(ARR) / sizeof(*ARR))

/**
 * \brief Pointer to a function that is used for comparing two items.
 *
 * The function should compare the two items and return a number less than,
 * equal to, or greater than zero if the first item is respectively less than,
 * equal to, or greater than the second item. The magnitude of the return value
 * does not matter, just the sign.
 *
 * \param [in] a The first item to compare.
 * \param [in] b The second item to compare.
 *
 * \return Returns an integer that is used to determine which item is smaller,
 * usually for sorting. Returns less than, equal to, or greater than zero if the
 * first argument is respectively less than, equal to, or greater than the
 * second argument.
 */
typedef int (*cmp_func)(const void *, const void *);


#endif /* end of include guard: _UTILS_H_ */

