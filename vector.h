/*
 * MIT License (MIT)
 *
 * Copyright (c) 2014 Brian Kubisiak <velentr.rc@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */



/*
 * vector.h
 *
 * Dynamically-resizable generic arrays, public interface.
 *
 * This file contains the public interface for dynamically-resizable arrays of
 * generic void pointers. The interface for this datastructure is similar to STL
 * vectors, but without templates or classes. In many cases, optimizes for speed
 * rather than checking inputs; garbage in, garbage out.
 *
 * Vectors using these functions are assumed to be dense. That is, there are no
 * gaps in between the valid elements in the vector. Also note that the order of
 * the elements within the vector is guarenteed to be constant, while the
 * indices are subject to change (e.g. if an element is deleted).
 *
 * Constants:
 *      DEF_VEC_SIZE    The default size for a vector, if no size is given on
 *                      initialization.
 *      VEC_ELEMSIZE    The size of each element in the vector.
 *
 * Datatypes:
 *      struct vector   Array-like data type that can store a set of void
 *                      pointers. The vector will expand as needed to store new
 *                      elements. The order of the elements in the vector will
 *                      remain constant.
 *      VecOperator     Function that operates on each element of a vector,
 *                      returning the new value for the element. This function
 *                      typedef is used for calls to 'vec_map'.
 *
 * Functions:
 *      vec_init    -- Initialize the memory for the vector. Must be called
 *                     before the vector can be used.
 *      vec_free    -- Free all the memory associated with the vector.
 *      vec_size    -- Gets the current number of elements stored in the vector.
 *      vec_space   -- Returns the number of elements that can currently be
 *                     stored in the vector. The vector will automatically
 *                     expand past this limit as needed.
 *      vec_resize  -- Resize the vector to the given size. This will be done
 *                     automatically as necessary, so it shouldn't generally
 *                     need to be called.
 *      vec_shrink  -- Shrink the vector so that it contains the absolute
 *                     minimum amount of memory needed to store all the
 *                     elements.
 *      vec_isempty -- Determines if the vector is empty (does not contain any
 *                     elements).
 *      vec_get     -- Get the element at the given index.
 *      vec_set     -- Set the element at the given index to the given value.
 *      vec_head    -- Get the first element in the vector.
 *      vec_tail    -- Get the last element in the vector.
 *      vec_push    -- Add an element to the end of the vector.
 *      vec_pop     -- Remove an element from the end of the vector.
 *      vec_delete  -- Get the element at the given index, and remove it from
 *                     the vector.
 *      vec_map     -- Apply a function to each element in the vector.
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_


/*
 * Used as the default size for a new vector if a size is not specified in the
 * init function.
 */
#define DEF_VEC_SIZE    32

/*
 * Defines the size of each element in the vector. Useful when converting
 * between number of bytes and number of elements.
 */
#define VEC_ELEMSIZE    sizeof(void *)



/*
 * struct vector
 *
 * Description: This structure contains all the variables and memory necessary
 *              for storing a dynamically-resizable array of void pointers. This
 *              is used mainly when the number of elements in the array is not
 *              known beforehand, or it is desirable to allow the array to grow
 *              with the data. Note that the order of the data stored in the
 *              vector will not change based on any of the operators in this
 *              file, but the indices may change.
 *
 * Members:     size    Stores a count of the total number of elements in the
 *                      vector.
 *              space   Stores the current number of elements that can fit in
 *                      the allocated array. Used for determining when the
 *                      vector must grow.
 *              data    Dynamically allocated array of void pointers for holding
 *                      any data type. Size may change as more elements are
 *                      added.
 *
 * Usage:       When using a vector, first call the 'vec_init' function to
 *              initialize the size and space, and to allocate a new buffer
 *              for the data. Then, any of the 'vec_*' functions may be called
 *              to operate on the vector.
 *
 *              None of the 'vec_*' functions will change the order of the
 *              elements in the vector; however, some may change to values of
 *              the elements, or change the indices of the elements (i.e. when
 *              an element is deleted).
 *
 *              When the vector is no longer needed, call the 'vec_free'
 *              function to free the buffer for this structure. If the elements
 *              of the vector are dynamically allocated, it might be necessary
 *              to use the 'vec_map' function to call a deallocator on each
 *              individual element before calling 'vec_free'. Once 'vec_free'
 *              has been called on the vector, then it should no longer be used,
 *              unless 'vec_init' is called on it again.
 */
struct vector {
    unsigned int size;
    unsigned int space;
    void **data;
};


/*
 * VecOperator
 *
 * Description: This indicates a pointer to a function that can be mapped over a
 *              vector. Takes in an element, the element's index, and a
 *              user-supplied argument. Returns a new element to replace the one
 *              that was passed as an argument. It can be used to implement a
 *              wide variety of filters, iterators, or accumulators. See the
 *              'vec_map' function for details of how this is used.
 *
 * Arguments:   e   Element to operate on.
 *              i   Index of the element being operated on.
 *              scr Scratch pointer passed to the function unmodified by the
 *                  'vec_map' function. This is used to pass other arguments to
 *                  the operator.
 *
 * Returns:     Returns a new element to be written back into the vector at the
 *              location of the current element (i.e. at the index i).
 *
 * Notes:       If a value needs to be returned outside the 'vec_map' function
 *              (e.g. if you are summing the elements), then use the scratch
 *              pointer to hold the return value.
 */
typedef void *(*VecOperator)(void *e, unsigned int i, void *scr);


/*
 * vec_init
 *
 * Description: Initialize the given vector so that it is ready to be used. This
 *              includes allocating the memory for the vector, as well as
 *              setting initial values for the member variables. The argument
 *              'v' is a pointer to the vector to initialize. The argument
 *              'size' is an initial value for the number of bytes in the
 *              vector, or 0 if the default size is desired. Returns a flag
 *              indicating whether or not the operation succeeded.
 *
 * Arguments:   v       Pointer to the vector to initialize.
 *              size    Initial value for the number of bytes in the vector. If
 *                      this argument is 0, then the default size will be used.
 *                      It is important to note that this is the number bytes,
 *                      not the number of elements.
 *
 * Returns:     If the memory is successfully allocated, returns 0. If the
 *              memory allocation fails, returns -1.
 *
 * Notes:       The 'size' argument shouldn't generally be used unless there is
 *              a specific reason to limit the size of the vector (or if the
 *              needed size of the vector is known beforehand). It should only
 *              be used when extreme performance is needed; you should probably
 *              just pass 0.
 *
 *              Also, it is the size in bytes, not the number of elements. To
 *              get the size for a vector with 'n' elements, pass 'n *
 *              sizeof(void *)'.
 */
int vec_init(struct vector *v, size_t size);

/*
 * vec_free
 *
 * Description: Free that memory used by the given vector. Does not actually
 *              change any of the members of the vector, just frees the buffer.
 *
 * Arguments:   v       Pointer to the vector to free.
 *
 * Returns:     None.
 *
 * Notes:       If any of the elements in the buffer need to be freed, they
 *              should be freed before calling this function. It is probably
 *              easiest to just call the 'vec_map' function with the element
 *              deallocator.
 */
void vec_free(struct vector *v);

/*
 * vec_size
 *
 * Description: Get the total number of elements currently stored in the given
 *              vector.
 *
 * Arguments:   v   Pointer to the vector to count the elements of.
 *
 * Returns:     Returns the current number of elements in the vector.
 *
 * Notes:       This returns the number of elements currently stored in the
 *              vector; it does not return how much space is available. Use
 *              'vec_space' to find how how space is currently available.
 *
 *              This function is extremely simple; it just returns one of the
 *              data members of the structure. It can easily be inlined.
 */
unsigned int vec_size(struct vector *v);

/*
 * vec_space
 *
 * Description: Gets the total number of elements that can currently be stored
 *              in the buffer. This is the number of elements that the vector
 *              can contain before it must be expanded to fit more elements.
 *
 * Arguments:   v   Pointer to the vector to count the total spaces of.
 *
 * Returns:     Returns the total number of elements that can be stored in the
 *              vector before it must be expanded.
 *
 * Notes:       Keep in mind this returns the number of elements that can fit in
 *              the buffer, not the number of bytes. This doesn't generally need
 *              to be called, because the vector will be expanded automatically
 *              when needed.
 *
 *              This is a very simple function, just returning a member of the
 *              struct. It can easily be inlined.
 */
unsigned int vec_space(struct vector *v);

/*
 * vec_resize
 *
 * Description: Resize the dynamically-allocated buffer holding the elements of
 *              the vector. The buffer of the given vector is reallocated to the
 *              given size (in bytes). Returns a flag indicating if the
 *              operation succeeded or not. If the memory allocation failed,
 *              returns -1. If the allocation succeeded, returns 0. Note that
 *              this does not check to make sure that the current elements of
 *              the buffer will fit in the new size; shrinking it too much will
 *              result in data loss.
 *
 * Arguments:   v       Pointer to the vector that needs to be resized.
 *              size    The new size for the buffer in bytes.
 *
 * Returns:     If the memory allocation succeeds, returns 0. If the allocation
 *              fails, returns -1.
 *
 * Notes:       If the passed size is 0, then the buffer will be freed. If the
 *              buffer inside the vector is NULL, then a new buffer will be
 *              allocated automatically. These are side effects of the 'realloc'
 *              library function being used.
 *
 *              Note that if the size is too small to hold all the elements
 *              currently in the buffer, data will be lost. Make sure to keep
 *              track of these elements elsewhere if this happens.
 */
int vec_resize(struct vector *v, size_t size);

/*
 * vec_shrink
 *
 * Description: Shrink the buffer used by the vector so that it is exactly large
 *              enough to hold all of its elements, without any wasted space. No
 *              data will be lost by calling this function. If the operation
 *              succeeds, returns 0. If the operation fails, returns -1.
 *
 * Arguments:   v   Pointer to the vector to shrink.
 *
 * Returns:     Returns a code indicating whether or not the memory allocation
 *              succeeded. If the allocation succeeded, returns 0. It it failed,
 *              returns -1.
 *
 * Notes:       It is unlikely (impossible?) that reallocating a buffer to a
 *              smaller size will fail, the return value is only included for
 *              completeness.
 *
 *              Also note that this guarentees that adding another element to
 *              the vector will result in a new memory allocation; calling this
 *              function would thus be wasteful unless you are running low on
 *              memory and you don't need to add anything more to the vector.
 *
 *              This function is a simple wrapper around 'vec_resize', so it can
 *              easily be inlined.
 */
int vec_shrink(struct vector *v);

/*
 * vec_isempty
 *
 * Description: Determines if the vector is empty. Returns true if the vector is
 *              empty; returns false if it is not empty.
 *
 * Arguments:   v   Pointer to the vector to check for emptiness.
 *
 * Returns:     If the number of elements in the vector is 0, returns true.
 *              Else, returns false.
 *
 * Notes:       Uses the traditional 'int' for the boolean, instead defining a
 *              new typedef or using the C99 'stdbool'. Does this for
 *              portability reasons mostly.
 *
 *              This function is very simple; it just compares a member to 0. It
 *              can easily be inlined.
 */
int vec_isempty(struct vector *v);

/*
 * vec_get
 *
 * Description: Gets the elements stored at the given index, and returns it.
 *              Note that no bounds checking is done, so make sure that the
 *              index is valid before calling this function.
 *
 * Arguments:   v   Pointer to the vector to get an element from.
 *              i   Index of the element to get from the vector.
 *
 * Returns:     Returns the void pointer stored at the given index in the
 *              buffer.
 *
 * Notes:       Be careful with this function. No bounds checking is done on the
 *              argument, so the returned data might be garbage if the index is
 *              out of range. Using an out of range index probably won't
 *              segfault, so make sure to bounds check before calling this.
 *
 *              This function is really simple, just gets an element from the
 *              buffer. It can probably be inlined.
 */
void *vec_get(struct vector *v, unsigned int i);

/*
 * vec_set
 *
 * Description: Set the element at the given index to the new value. This does
 *              not add a new element to the vector, it just modifies an
 *              existing element. However, no bounds checking is done by this
 *              function, so make sure the index is valid before calling this.
 *
 * Arguments:   v   Pointer to the vector in which to modify an element.
 *              i   Index of the element to modify.
 *              e   New element value to store at the given index.
 *
 * Notes:       This does not do any bounds checking, which can lead to bad
 *              behavior if the passed index is beyond the bounds of the buffer.
 *              Make sure to do bounds checking before calling this function.
 *
 *              Also note that the pointer at the given index will be
 *              overwritten; if there is unfreed memory there, make sure to
 *              'vec_get' the element before calling this function, or the
 *              pointer will be lost forever.
 *
 *              This function is very simple (just stores a pointer in the
 *              buffer). It can easily be inlined.
 */
void vec_set(struct vector *v, unsigned int i, void *e);

/*
 * vec_head
 *
 * Description: Get the first element in the vector, or NULL is the vector is
 *              empty. This will explicitly check that the vector is not empty
 *              before returning, so there is no way for this function to return
 *              garbage.
 *
 * Arguments:   v   Pointer to the vector from which to get the head.
 *
 * Returns:     If the vector is not empty, returns the void pointer stored at
 *              index 0 in the vector. If the vector is empty, returns NULL.
 *
 * Notes:       This function will do bounds checking; that is, it will
 *              explicitly check that the vector is not empty before getting the
 *              element. Make sure to check for a NULL pointer returned from
 *              this function, indicating the vector is empty.
 */
void *vec_head(struct vector *v);

/*
 * vec_tail
 *
 * Description: Get the last (highest indexed) element in the vector, or NULL if
 *              the vector is empty. This function does explicit bounds
 *              checking, so it will never return garbage from a valid vector.
 *
 * Arguments:   v   Pointer to the vector from which to get the last element.
 *
 * Returns:     Returns the last valid element in the vector. This is the
 *              element stored in the buffer with the highest index. If the
 *              vector is empty, returns NULL.
 *
 * Notes:       Does explicit bounds checking, so make sure to check for a NULL
 *              return value if the vector is empty.
 */
void *vec_tail(struct vector *v);

/*
 * vec_push
 *
 * Description: Adds a new element to the end of the vector. The new element
 *              will be at the highest index of the vector. If there is not
 *              enough space in the buffer for the new element, then the buffer
 *              will be resized so the new element will fit. If this resize
 *              fails (i.e. failed memory allocation), then -1 is returned and
 *              the element is not added; the vector is left unchanged. If the
 *              element is successfully added, then 0 is returned.
 *
 * Arguments:   v   Pointer to the vector to which to add an element.
 *              e   Element to add to the end of the vector.
 *
 * Returns:     If the element is successfully added, returns 0. If the vector
 *              needs to be resized to fit the element, and the resize fails,
 *              returns -1.
 *
 * Notes:       The state of the vector is not changed if the resize operation
 *              fails.
 */
int vec_push(struct vector *v, void *e);

/*
 * vec_pop
 *
 * Description: Removes the last (highest indexed) element from the vector,
 *              returning its value. If the vector is empty, returns NULL and
 *              does not change the state of the vector. Note that the vector
 *              will not be resized, no matter how many elements are popped,
 *              unless 'vec_shrink' is called.
 *
 * Arguments:   v   Pointer to the vector from which to pop an element.
 *
 * Returns:     Returns the tail of the vector. This is the element in the
 *              buffer with the highest index.
 *
 * Notes:       Make sure to check for a NULL return value if the vector is
 *              empty. If you want to reclaim memory after a lot of elements are
 *              popped, call the 'vec_shrink' function.
 */
void *vec_pop(struct vector *v);

/*
 * vec_delete
 *
 * Description: Deletes an element from the vector. This function will remove
 *              the element at the given index from the vector, returning the
 *              element so that it is not lost. This operation will then shift
 *              the buffer to overwrite the deleted element. This shift is
 *              needed in order to keep the vector dense while preserving the
 *              order of the elements. Note that this will change the index of
 *              every element after the given element. If the given index was
 *              not valid (i.e. it is too large), then NULL is returned and the
 *              vector is unchanged.
 *
 * Arguments:   v   Pointer to the vector from which to delete an element.
 *              i   Index of the element to delete.
 *
 * Returns:     Returns the deleted element, or NULL if the given index was out
 *              of range.
 *
 * Notes:       This is a slow and ugly function because of the large memory
 *              movement. It also messes with the indices of the vector in order
 *              to keep it dense and ordered. You probably shouldn't use this
 *              function unless you really have to.
 */
void *vec_delete(struct vector *v, unsigned int i);

/*
 * vec_map
 *
 * Description: Apply an operator to each element in a vector. Replace each
 *              element with the return value from the operator. The operator
 *              takes the element, its index, and a scratch pointer as
 *              arguments. This can be used to implement a wide range of
 *              iterators over the vector.
 *
 * Arguments:   v       Pointer to the vector to iterate over.
 *              op      Operation to perform on each element.
 *              scratch Scratch pointer to pass as an argument to the operation.
 *
 * Notes:       This is the preferred way of iterating over the vector, as using
 *              this does not make assumptions about the internals of the
 *              vector. See the documentation for the VecOperator type for more
 *              information about the format of the operator.
 */
void vec_map(struct vector *v, VecOperator op, void *scratch);


#endif /* end of include guard: _VECTOR_H_ */
