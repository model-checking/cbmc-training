# CBMC as proof

Think of CBMC as a tool for doing mathematical proof. CBMC does an
exhaustive search for issues affecting functional correctness and
memory safety.  If CBMC finds an issue, [as we have seen](debugging.md),
then CBMC has likely found a bug.
But if CBMC does *not* find any issues, then you can rest assured
that the issues like memory safety that you are asking CBMC to check for
will not occur in your program with the inputs you are asking CBMC to check.

We note, however, that using CBMC may require bounding the size of the inputs
you are asking CBMC to check.
It may be necessary, for example, to assume that packets
coming off the network are of size at most 1024 bytes
in order to fit the problem into CBMC.
We refer to assumptions like this as *proof assumptions*.
It is important to remember that proof assumptions exists,
and to communicate them clearly to the reader or consumer of
any proof you write with CBMC.

Let's look again at [memory-safety.c](examples/simple/memory-safety.c)
```c
#define SIZE 20
char buffer[SIZE];

char read_buffer(int i)  { return buffer[i];     }
char read_pointer(int i) { return *(buffer + i); }

main() {
  int index;
  read_buffer(index);
  read_pointer(index);
}
```
that we found to have [memory safety issues](debugging.md#memory-safety)
in the last section.  The problem was obviously the lack of bounds checking
in the code.  Let's fix that.

## Memory safety proof

Let's try adding bounds checking.
Consider [memory-safety1.c](examples/simple/memory-safety1.c)
```c
#include <stdlib.h>

#define SIZE 20
char buffer[SIZE];

char read_buffer(int i)  {
  if (i < SIZE) return buffer[i];
  return NULL;
}
char read_pointer(int i) {
  if (i < SIZE) return *(buffer + i);
  return NULL;
}

main() {
  int index;
  read_buffer(index);
  read_pointer(index);
}
```
We probably want better error handling than just returning `NULL`,
but let's run CBMC to prove memory safety:
```bash
cbmc memory-safety1.c --bounds-check --pointer-check
```
```
** Results:
memory-safety1.c function read_buffer
[read_buffer.array_bounds.1] line 10
  array 'buffer' lower bound in buffer[(signed long int)i]: FAILURE
[read_buffer.array_bounds.2] line 10
  array 'buffer' upper bound in buffer[(signed long int)i]: SUCCESS

memory-safety1.c function read_pointer
[read_pointer.pointer_dereference.1] line 14
  dereference failure: pointer outside object bounds in buffer[(signed long int)i]:
  FAILURE

** 2 of 3 failed (2 iterations)
VERIFICATION FAILED
```
Oh, nuts.  We failed.  We added bounds checking only for the upper bound.

## Memory safety proof 2.0

Let's try adding bounds checking for both the upper and lower bounds.
Consider [memory-safety2.c](examples/simple/memory-safety2.c)
```c
#include <stdlib.h>

#define SIZE 20
char buffer[SIZE];

char read_buffer(int i)  {
  if (0 <= i && i < SIZE) return buffer[i];
  return NULL;
}
char read_pointer(int i) {
  if (0 <= i && i < SIZE) return *(buffer + i);
  return NULL;
}

main() {
  int index;
  read_buffer(index);
  read_pointer(index);
}
```
and run CBMC
```bash
cbmc memory-safety2.c --bounds-check --pointer-check
```
```
** 0 of 3 failed (1 iterations)
VERIFICATION SUCCESSFUL
```
and declare victory.

## Memory safety proof harness

Let's pivot back to the notion of a proof harness before we declare
complete victory.

Back when we introduced [CBMC as a form of unit testing](unit-testing.md),
we had a function [quartile](quartile.c) under test
and a [unit test](unit-test.c) for `quartile`.
The unit test set up the software environment for the function under test
and invoked the function.
The function `quartile` was so simple that all the unit test had to do
was initialize the argument `x` passed to `quartile` and invoke
`quartile` on `x`.
Then we removed the initialization of `x` so that CBMC could consider
all possible integer values for the argument `x`.
We transformed the unit test into what we call a *proof harness* for
the function `quartile` and used that with CBMC.

We routinely find ourselves with a library and wanting to use
CBMC to prove that all of the public entry points in the library's
API are memory safe.
Our style is to write a memory safety proof for each entry point
independently.
We write one proof harness for each entry point.  The purpose
of the proof harness is to model the software environment in which
the entry point will be called.  The proof harness will always have
to model the inputs to the entry point.  But the proof harness might
also have to model some aspects of the global state if, for example,
there is a system table accessed by the entry point.

In the ideal world,
a proof harness will construct arbitrary, unconstrained values for
every variable and data structure in the software environment of an
entry point, and invoke the entry point.
In the real world,
the entry point makes some assumptions about its inputs.
It may, at the very least, assume that the system table is well-formed
in the sense that it satisfies some data structure invariant.
In the ideal world, the proof author will talk with the code author
and convince the developer to write more bullet-proof code.
In the real world,
the proof author will model these assumptions in the proof harness
to prove memory safety.
But now these assumptions become part of the proof:
CBMC has demonstrated that the entry point is memory safe,
but only when the assumptions described in the proof harness are true when
the entry point is invoked.
A developer might want to include in the testing framework some
checks that these assumptions really are true at all call sites calling
the entry point.
Or a developer might want to use CBMC to prove this...

But let's return to our memory safety example and transform it into
something you are more likely to encounter in the real world.

### The library

First, the buffer and the accessor functions are likely to
be part of a library.
Here is [library.h](examples/simple/library.h)
```c
char read_buffer(int i);
char read_pointer(int i);
```
and [library.c](examples/simple/library.c)
```c
#include <stdlib.h>
#include "library.h"

#define SIZE 20
char buffer[SIZE];

char read_buffer(int i)  {
  if (0 <= i && i < SIZE) return buffer[i];
  return NULL;
}
char read_pointer(int i) {
  if (0 <= i && i < SIZE) return *(buffer + i);
  return NULL;
}
```

### The proof harnesses

Second, we write one proof harness for each entry point.
Here is [read_buffer_harness.c](examples/simple/read_buffer_harness.c)
```c
#include "library.h"

harness() {
  int index;
  read_buffer(index);
}
```
and [read_pointer_harness.c](examples/simple/read_pointer_harness.c)
```c
#include "library.h"

harness() {
  int index;
  read_pointer(index);
}
```
Notice that we have changed the function name from `main` to `harness` in
these proof harnesses.  This is not essential, it is just a matter of style,
but it does avoid name conflicts with a real `main` that might exist in
the real environment.

### The proof

Now we can run the proofs just as before, giving both the library
and the proof harness on the command line, and also indicating to CBMC
that the function to test is now called `harness` and not `main`:
```bash
cbmc library.c read_buffer_harness.c --bounds-check --pointer-check --function harness
cbmc library.c read_pointer_harness.c --bounds-check --pointer-check --function harness
```
In both cases:
```
** 0 of 3 failed (1 iterations)
VERIFICATION SUCCESSFUL
```
Success!!


### Building code for proof

One last thing.
In the real world, the library is not this simple.
There is probably a build process to compile the source code into a library
using a compiler like `gcc`.

One approach is to build the library using `goto-cc` in place of `gcc`.
The compiler `goto-cc` is intended to be a drop-in replacement for `gcc`.
Using `goto-cc` should be as easy as building with `CC=goto-cc` in place
of `CC=gcc`.
The difference is that `goto-cc` produces an intermediate representation for
the program called
a "goto program" or "goto binary" that is used by CBMC for model checking.
When we run `cbmc`
directly on a source file, CBMC is invoking `goto-cc` on the source file
before model checking, but we can run `goto-cc` ourselves to build the
goto program, and run CBMC on that.

Our approach, however, is not to build the entire library.  There are
performance and complexity issues that we won't go into here related to
symbolic execution and constraint solving.
We usually build only the portions of the library that are required to
prove that a particular entry point is memory safe.

Using our simple library above to illustrate, we would build and prove
memory safe the entry point `read_buffer` as follows:
```bash
goto-cc library.c -o library.goto
goto-cc read_buffer_harness.c -o read_buffer_harness.goto
goto-cc library.goto read_buffer_harness.goto -o read_buffer.goto --function harness
cbmc read_buffer.goto --bounds-check --pointer-check
```
The first two lines build the source files, the third links them
together (and identifies `harness` as the entry point), and the fourth line
runs CBMC on the result.  The process for `read_pointer` is similar.

Now, finally, and forever:
```
** 0 of 3 failed (1 iterations)
VERIFICATION SUCCESSFUL
```
Success!!