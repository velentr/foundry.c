.. Documentation for a vector.

Vector
======

Dynamically-resizable generic arrays.

Description
-----------

Vectors implement dynamically resized arrays of similar objects. The interface
for this data structure is similar to STL vectors, but without templates or
classes.

Vectors using these functions are assumed to be dense. That is, there are no
gaps in between the valid elements in the vector. Note that the order and
indices of the elements in the vector are guaranteed to remain the same (unless
the caller explicitly calls :c:func:`vec_swap`).

Adding elements to the vector will perform a shallow copy to duplicate the
object in an internal buffer; it is thus acceptable to pass elements to
:c:func:`vec_set` et al. that will immediately be freed. The vector will manage
its own memory.

Synopsis
--------

.. code-block:: c

    #include <stdlib.h>

    #include "vector.h"

    int main()
    {
        struct vector v;

        vec_init(&v, sizeof(int), 0, realloc);

        vec_destroy(&v);
    }

API Reference
-------------

.. c:macro:: DEF_VEC_LEN

    Default length for a vector, if none is provided on initialization.

.. c:type:: struct vector

    Array-like data type that can store a set of objects.

    This structure contains all the variables and memory necessary for storing a
    dynamically-resizable array of objects. This is used mainly when the number
    of elements in the array is not known beforehand, or it is desirable to
    allow the array to grow with the data.

    When using a vector, first call the :c:func:`vec_init` function to allocate
    a new buffer for the data. Then, any of the ``vec_*()`` functions may be
    called to operate on the vector.

    When the vector is no longer needed, call the :c:func:`vec_destroy` function
    to free the buffer for this structure. If the elements of the vector are
    dynamically allocated, it might be necessary to use the :c:func:`vec_map`
    function to call a deallocator on each individual element before calling
    :c:func:`vec_destroy`. Once :c:func:`vec_destroy` has been called on the
    vector, then it should no longer be used, unless :c:func:`vec_init` is
    called on it again.

    .. c:member:: char *data

        Dynamically-allocated array for holding data.

    .. c:member:: void *(*realloc)(void *, size_t)

        Allocator for getting more memory.

    .. c:member:: size_t len

        Count of the number of elements in the vector.

    .. c:member:: size_t elemsize

        Size of each element in the vector.

    .. c:member:: size_t space

        Number of elements that can currently fit in the allocated memory.

.. c:type:: void vec_operator(void *e, size_t i, void *scratch)

    Function that operates on each element of a vector.

    A vector operator is used for mapping a function over a vector. The function
    should take as arguments an element, the element's index, and a
    user-supplied scratch argument. It can be used to implement a wide variety
    of filters, iterators, or accumulators. See the :c:func:`vec_map` function
    for details of how this is used.

    :param e: Element to operate on.
    :param i: Index of the element being operated on.
    :param scratch: Scratch pointer passed through the :c:func:`vec_map`
        function.

.. c:function:: int vec_init(struct vector *v, size_t elemsize, size_t len, \
    void *(*alloc)(void *, size_t))

    Initialize the memory for the vector. Must be called before the vector
    can be used.

    Initialize the given vector so that it is ready to be used. This includes
    allocating the memory for the vector, as well as setting initial values for
    the member variables. The argument ``v`` is a pointer to the vector to
    initialize. The argument ``elemsize`` is the size of each individual element
    in the vector. The argument ``len`` is an initial value for the number of
    objects in the vector, or 0 if the default length (:c:macro:`DEF_VEC_LEN`)
    is desired. Returns a flag indicating whether or not the operation
    succeeded.

    Note that the ``len`` argument shouldn't generally be used unless there is a
    specific reason to limit the size of the vector (or if the needed size of
    the vector is known beforehand). It should only be used when extreme
    performance is needed; you should probably just pass 0.

    :param v: Pointer to the vector to initialize.
    :param elemsize: Size of each element in the vector.
    :param len: Initial value for the number of elements in the vector. If this
        argument is 0, then the default size will be used.
    :param alloc: Memory allocator used for performing all allocation for the
        vector. Interface should be equivalent to ``realloc()``.

    :return: If the memory is successfully allocated, returns 0. If the memory
        allocation fails, returns -1.

.. c:function:: int vec_resize(struct vector *v, size_t size)

    Resize the dynamically-allocated buffer holding the elements of the vector.

    This will be called automatically when needed, so it generally does not need
    to be called by the user. The buffer of the given vector is reallocated to
    the given size (in bytes). Returns a flag indicating if the operation
    succeeded or not. If the memory allocation failed, returns -1. If the
    allocation succeeded, returns 0. Note that this does not check to make sure
    that the current elements of the buffer will fit in the new size; shrinking
    it too much will result in data loss.

    Note that if the passed size is 0, then the buffer will be freed. If the
    buffer inside the vector is ``NULL``, then a new buffer will be allocated
    automatically. These are side effects of the ``realloc()`` library function
    being used.

    Note that if the size is too small to hold all the elements currently in the
    buffer, data will be lost. Make sure to keep track of these elements
    elsewhere if this happens.

    :param v: Pointer to the vector that needs to be resized.
    :param size: The new size for the buffer in bytes.

    :return: If the memory allocation succeeds, returns 0. If the allocation
        fails, returns -1.

.. c:function:: void *vec_head(const struct vector *v)

    Get the first element in the vector, or ``NULL`` if the vector is empty.

    This will explicitly check that the vector is not empty before returning, so
    there is no way for this function to return garbage.

    :param v: Vector to get the head from.

    :return: If the vector is not empty, returns a pointer to the element stored
        at index 0 in the vector. If the vector is empty, returns ``NULL``.

.. c:function:: void *vec_tail(const struct vector *v)

    Get the last (highest indexed) element in the vector, or ``NULL`` if the
    vector is empty.

    This function does explicit bounds checking, so it will never return garbage
    from a valid vector.

    :param v: Vector to get the tail from.

    :return: Returns the last valid element in the vector. This is the element
        stored in the buffer with the highest index. If the vector is empty,
        returns ``NULL``.

.. c:function:: int vec_push(struct vector *v, const void *e)

    Add a new element to the end of the vector.

    The new element will be at the highest index of the vector. If there is not
    enough space in the buffer for the new element, then the buffer will be
    resized so the new element will fit. If this resize fails (i.e. failed
    memory allocation), then -1 is returned and the element is not added; the
    vector is left unchanged. If the element is successfully added, then the
    index of the new element is returned.

    :param v: Vector to which the new element is added.
    :param e: Element to add at the end of the vector.

    :return: If the element is successfully added, returns the index of the new
        element. If the vector needs to be resized to fit the element and the
        resize fails, returns -1.

.. c:function:: int vec_pop(struct vector *v)

    Removes the last (highest indexed) element from the vector.

    If the vector is empty, returns -1 and does not change the state of the
    vector. Note that the vector will not be resized, no matter how many
    elements are popped, unless :c:func:`vec_shrink` is called.

    :param v: Vector from which to pop an element.

    :return: If the operation succeeds (i.e. the vector is nonempty), returns 0.
        If the operation fails, returns -1.

.. c:function:: void vec_map(struct vector *v, vec_operator op, void *scratch)

    Apply an operator to each element in a vector.

    The operator takes the element, its index, and a scratch pointer as
    arguments. This can be used to implement a wide range of iterators over the
    vector.

    Note that this is the preferred way of iterating over the vector, as using
    this does not make assumptions about the internals of the vector. See the
    documentation for the :c:type:`vec_operator` type for more information about
    the format of the operator.

    :param v: Vector to map the function over.
    :param op: Operator to map over the function.
    :param scratch: Scratch argument passed directly to ``op`` while iterating.

.. c:function:: void vec_swap(struct vector *v, size_t i, size_t j)

    Swap two elements in the vector.

    This will set the value at index ``i`` to the value at index ``j`` and vice
    versa. Note that this operation may be dangerous if there are pointers to
    either element ``i`` or element ``j`` elsewhere in the code; make sure that
    other parts of calling code understand that the elements are swapped.

    :param v: Vector containing the elements to swap.
    :param i: Index of the first element to swap.
    :param j: Index of the second element to swap.

.. c:function:: void vec_destroy(struct vector *v)

    Free all memory associated with the vector.

    Deallocate the internal buffer used by the given vector. This will change
    some members of the vector, but the vector should not be used after being
    freed, so this shouldn't matter. Note that accessing any previous references
    retrieved with :c:func:`vec_get` is undefined behavior after this function
    is called.

    Note that any elements in the vector that need to be destroyed should be
    freed before calling this function. This should be done with the
    :c:func:`vec_map` function.

    :param v: Vector to destroy.

.. c:function:: size_t vec_len(const struct vector *v)

    Get the total number of elements currently stored in the given vector.

    :param v: Vectro to count the elements of.

    :return: Returns the current number of elements in the vector.

.. c:function:: size_t vec_space(const struct vector *v)

    Gets the total number of elements that can currently be stored in the
    internal buffer.

    This is the number of elements that the vector can contain before it must be
    expanded to fit more elements. This generally won't need to be used except
    in case of performance tuning.

    :param v: Vector to calculate the total space of.

    :return: Returns the total number of elements that can fit in the vector.

.. c:function:: int vec_shrink(struct vector *v)

    Shrink the vector.

    Shrink the internal buffer used by the vector so that it is exactly large
    enough to hold all of its elements, without any wasted space. No data will
    be lost by calling this function. If the operation succeeds, returns 0. If
    the operation fails, returns -1.

    Note that this guarentees that adding another element to the vector will
    result in a new memory allocation; calling this function would thus be
    wasteful unless you are running low on memory and you don't need to add
    anything more to the vector.

    :param v: Vector to shrink.

    :return: Returns 0 if the allocation succeeds. Returns -1 if the allocation
        fails.

.. c:function:: int vec_isempty(const struct vector *v)

    Determines if the vector is empty.

    :param v: Vector to check for emptiness.

    :return: If the number of elements in the vector is 0, returns nonzero
        (true). Else, returns zero (false).

.. c:function:: void *vec_get(const struct vector *v, size_t i)

    Gets the element stored at the given index.

    Note that no bounds checking is done, so make sure that the index is valid
    before calling this function; failure to do so may result in undefined
    behavior.

    :param v: Vector to get an element from.
    :param i: Index of the element to get from the vector.

    :return: Returns a pointer to the requested element.

.. c:function:: void vec_set(struct vector *v, size_t i, const void *e)

    Set the element at the given index to the new value.

    This does not add a new element to the vector, it just modifies an existing
    element. However, no bounds checking is done by this function, so make sure
    the index is valid before calling this.

    Note that the memory at the given element will be overwritten. If there is
    unfreed memory there, make sure to destroy the element from
    :c:func:`vec_get` before setting the element.

    :param v: Vector in which to modify an element.
    :param i: Index of the element to modify.
    :param e: New element value to store at the given index.

Internal API Reference
----------------------

.. c:function:: static int _vec_checkspace(struct vector *v)

    Check if the vector has enough space to grow.

    Check the space of the vector to ensure that another element can be added.
    If there is not enough space to add another element, then resize the vector.
    If the resize fails, returns -1 without changing the vector. Else, returns
    0.

    This function is used so that the vector will grow according to some
    predefined rule. Any function that needs to expand the vector should use
    this function rather than calling resize directly.

    Currently, the growth rule doubles the amount of space in the vector if it
    is needed.

    :param v: Vector to check the space of.

    :return: Returns 0 if the vector currently contains enough space to add
        another element at the tail (or was successfully resized to create
        enough space). Returns -1 if a resize was attempted, but it did not
        succeed.

