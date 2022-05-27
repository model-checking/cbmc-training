# Proof assumptions

A proof assumption is anything that limits the applicability of a
statement that has been otherwise proved by CBMC.
An example of a proof assumptions is the assumption that packets coming
off a network are of size at most 1024 bytes.
This proof assumption would arise if the proof harness for a function
modeled an incoming packet as any character buffer of
size at most 1024 characters.  Suppose that we run CBMC with this proof
harness, and CBMC is unable to find any memory safety errors.  We might
be tempted to say that CBMC has proved the function is memory safe.  But
the truth is that CBMC has proved the function is memory safe only when
the function is invoked on a packet of size at most 1024 characters.
We believe that such a proof has value, and that hard-to-find software
issues will be discovered in the course of writing such a proof, but
the result is not absolute proof of memory safety.

Just to clear, every proof in mathematics depends on proof assumptions.
In number theory, a theorem may hold for all integers `n`, assuming `n`
is nonzero.  The theorem is powerful because it is applicable to nearly
every integer.  In cryptography, a theorem may hold only if `n` is the product
of two prime numbers `p` and `q` of roughly the same size.  The theorem
may still have powerful applications, but it applies only to a much smaller
set of integers.

Proof assumptions must be clearly and accurately communicated to any
proof reader and to any consumer of the proof.
Doing this right may be the hardest part of proof.
It is particularly important in the context of software proof, however,
because the credibility of the proof tool and the methodology for using the
tool depends on getting this right.
Trust is hard to earn and easy to lose.

The two most common sources of proof assumptions are
[memory allocation](#memory-allocation) and
[function abstraction](#function-abstraction).

## Memory allocation

There are two common proof assumptions about memory.

### Size

One class of assumption is size.  Some are obvious.  If the proof harness
includes
```C
  size_t size;
  __CPROVER_assume(size < 100);

  char *buffer = (char *) malloc(size);
```
then the buffer is assumed to be of size smaller than 100.
Some are implicit.  If the proof harness includes
```C
  size_t size;

  char *buffer = (char *) malloc(size);
```
then the buffer is implicitly bounded to the size of the maximum object
that CBMC can model (which is probably smaller than a 64-bit integer, see
our discussion of the [memory model](memory-model.md) for more information).

### Invariants

One class of assumption is well-formedness or implicit data structure
invariants.
Suppose the proof harness includes
```C
  typedef struct {
    size_t size;
    char *buffer;
  } buffer_t;

  buffer_t *buffer = (buffer_t *) malloc(sizeof(*buffer));
  buffer->buffer = (char *) malloc(buffer->size);
```
then the proof harness is implicitly assuming that the pointer `buffer->buffer`
is either NULL or a valid pointer to a buffer of size `buffer-size`.  This
is implicitly assuming an data structure invariant about `buffer`.
Is this really something you want to assume about a buffer you receive as
input from an untrusted adversary.  Is this really something want to assume
even when `buffer->size == 0`?

## Function abstraction

There are two common kinds of function abstraction with CBMC.

### Missing functions

One is implicit function abstraction done automatically by CBMC.

When CBMC encounters
the invocation of a function `missing` and cannot find a function definition
for `missing`, CBMC issues a warning "no function body for `missing`" and
models the function `missing` as a function that

* has no side-effects (makes no change to the state), and
* returns an arbitrary, unconstrained value of the appropriate return type.

This assumption of no side-effects is a serious assumption.  Almost no
functions in C are purely functional.  On the other hand, it is
an easy way to model something like a random number generator returning
a `unint64_t` used in some cryptographic code under test.
Including the actual function definition of the random number generator
will probably be too much for CBMC, but by leaving the function
undefined (omitting the code defining the random number generator
from the build), CBMC will consider the impact on the property being
testing of every unsigned, 64-bit integer value that can possibly
be returned by the number generator.

### Stubs

One is explicit function abstraction written by the proof author for the proof.

Consider a cryptographic key generator
```C
  int key_generator(void *context, public_key_t *public, private_key_t *private)
```
As with the random number generator, including the code for a cryptographic
key generator may be too much for CBMC.  Reconstructing cryptographic keys is,
after all, designed to be intractable!

A reasonable stub for the key generator is:
```C
  int key_generator(void *context, public_key_t *public, private_key_t *private)
  {
    __CPROVER_havoc(public);
    __CPROVER_havoc(private);
    int rc;
    return rc;
  }
```
The `__CPROVER_havoc` function fills the objects pointed to by `public`
and `private` with arbitrary, unconstrained data.

This stub has the pleasing effect of considering possible every value
(well-formed or not) that could be written to `public` and `private`
by the generator,
and considering possible every integer value that could be used as a
return code.

This stub is, however, making the implicit assumption that the pointers
are valid.  A better stub would check that the function is being called
correctly:
```C
  int key_generator(void *context, public_key_t *public, private_key_t *private)
  {
    __CPROVER_assert(context, "context is nonnull");
    __CPROVER_assert(public, "public is nonnull");
    __CPROVER_assert(private, "private is nonnull");

    __CPROVER_havoc(public);
    __CPROVER_havoc(private);
    int rc;
    return rc;
  }
```
