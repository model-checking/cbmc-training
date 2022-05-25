# How do memory pointers work?

A pointer in C points to a memory location.  It is a string of 32 or
64 bits interpreted as an unsigned integer denoting a memory
address.  CBMC, however, interprets this same string of bits as object id
followed by an offset into the object denoted by the object id.

CBMC models the heap as a collection of objects.  Each object has an
id and a size.  A pointer consists of an object id and an offset into
the object.  CBMC encodes the id and offset into a C pointer by using the
top bits for the object id and the bottom bits for the offset into the object.
By default, CBMC uses the top 8 bits for the object id and the bottom
56 bits for the offset in 64-bit code (and the bottom 24 bits in 32-bit code).

**Object id**:
The number of bits used to represent the object id is 8 by default.
It can be changed to `K` with the flag `--object-bits K` that is passed
to the compiler `goto-cc` and to `cbmc` itself.
It is important that the same value `K` be passed to `goto-cc` and `cbmc`.
If you are using the CBMC starter kit, you set `K` in your Makefile with
```
CBMC_OBJECT_BITS = K
```

You may see CBMC fail with an error message saying that CBMC tried to
allocation "too many objects."
One option is to increase the number of
object bits and see if the problem goes way.  Even if this works, it is
worth getting your hands dirty and trying to understand why CBMC
needs to allocate so many objects for your code.
Remember that CBMC is exploring all
paths through your code, and has to model all of the objects allocated
in all of these paths.
In the best case, you will discover a way to reduce the number of objects
produced by your code, and produce better code.
In the worst case, you may find it prudent to bound the size of a data
structure (maybe the size of an input buffer) to bound the number of
objects CBMC has to model.

**Object offset**:
The number of bits used to represent the offset into the object is
the number of remaining bits in a pointer.  This is 56 by default in
64-bit code and 24 by default in 32-bit code.

* The maximum offset is the maximum size of an object that can be
  modeled in CBMC.  This maximum size is smaller than the maximum size
  that can (at least theoretically) be used in a C program.  This is a
  proof assumption, and it needs to be made clear to the readers or
  customers of a proof who need to know exactly what CBMC has proved
  about the code.  If you are using the CBMC starter kit, you can access
  this maximum size using the preprocessor definition
  `CBMC_MAX_OBJECT_SIZE` as in the code fragment
  ```
  size_t size;
  char *ptr;

  __CPROVER_assume(size <= CBMC_MAX_OBJECT_SIZE);
  ptr = (char *) malloc(size);
  ```

* The offset is a *signed* integer.  In pointer arithmetic, the
  difference `ptr1 - ptr2` can be negative or positive depending on
  whether `ptr1` points before or after `ptr2`.  This means that the
  maximum size of an object with a 56- or 24-bit offset is actually
  the maximum positive number that can be represented in 55 or 23
  bits.  If you are using the CBMC starter kit, this maximum positive
  number is `CBMC_MAX_OBJECT_SIZE`.

CBMC provides a few [intrinsics](https://github.com/diffblue/cbmc/blob/develop/src/ansi-c/cprover_builtin_headers.h) that can be useful in a proof harness:
* `__CPROVER_POINTER_OBJECT(ptr)` is the id of the object denoted by ptr.
* `__CPROVER_POINTER_OFFSET(ptr)` is the offset into the object denoted by ptr.
* `__CPROVER_OBJECT_SIZE(ptr)` is the size of the object denoted by ptr.
* `__CPROVER_same_object(ptr1, ptr2)` is true if ptr1 and ptr2 point to
  the same object.
* `__CPROVER_is_invalid_pointer(ptr)` is true if ptr is a valid pointer.

CBMC provides a few [functions](https://github.com/diffblue/cbmc/blob/develop/src/ansi-c/cprover_builtin_headers.h) that can be useful in a proof harness.  These
functions havoc an object, which means they set all or some of the object
to an arbitrary, unconstrained value.  The word "havoc" means
"widespread destruction".  To "wreak havoc" means "to cause great damage".
We havoc an object to search for an object value that will induce an
issue in our code like a memory safety error or integer overflow.
* `__CPROVER_havoc_object(ptr)` havocs the entire object denoted by the
  object id in `ptr`.
* `__CPROVER_havoc_slice(ptr, length)` havocs the portion of the object
  starting at the offset denoted by the object offset in `ptr`
  and extending   for `length` bytes.
