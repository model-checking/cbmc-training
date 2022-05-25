# How does malloc work?

CBMC has several models of `malloc`.

In one model of `malloc` (the default model), the function never fails
to allocate an object on the heap and always returns a valid pointer to a
valid object.  In particular, `malloc` will never return `NULL`,
`malloc` will never run out of memory, and `malloc` will never fail.

This is a problem if you are using CBMC to check for memory safety issues,
because `malloc` can fail and return `NULL`, and trying to dereference a
null pointer is one of the issues you want to check for.

In other models of `malloc`, the function can nondeterministically
choose to fail and return `NULL`, and the function will fail if it is
asked to allocate an object that is larger than CBMC can model (see
the notion of object bits in the discussion of the
[CBMC memory model](memory-model.md)).  These models of `malloc` are used
when CBMC is invoked with the
flags `--malloc-may-fail` and `--malloc-fail-null`.

We recommend that you use `--malloc-may-fail` and `--malloc-fail-null`.
Do not use the default model of `malloc`.
The CBMC starter kit uses both flags by default.

One last comment about `malloc`: The invocation `malloc(0)` can return
a valid pointer to nothing.
The C standard says that invoking `malloc` with size 0
should return a pointer that can be passed to `free` but cannot be
dereferenced.
