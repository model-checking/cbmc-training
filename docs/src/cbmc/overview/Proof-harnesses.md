# Proof harnesses

This section is about writing proof harnesses for CBMC.
Our [proof harness overview](proof.md#memory-safety-proof-harness)
has already introduced the notion of a proof harness,
and this section goes into greater detail about the issues to consider
when writing a harness.
The purpose of a proof harness is to build a model of the environment of
the function under test.  The purpose of this model is to collect in one
place all of the assumptions required for the proof to hold.  The proof
harness is responsible for:
* [Modeling input](#modeling-input)
* [Modeling global state](#modeling-global-state)
* [Modeling interfaces](#modeling-interfaces)
* [Allocating data](#allocating-data)
* [Validating data](#validating-data)
* [Proof assumptions](#proof-assumptions)

## Modeling input

For simple functions, the model of the environment is just the input to the
function.


Suppose we have a library [library1.c](examples/harness/library1.c)

```c
#include <assert.h>

int alpha1(int a) {
    return a+1;
}

int beta1(int b) {
    int bb = alpha1(b);
    assert(bb == b);
}
```

and we want to test function `beta1`.  The input to function `beta1`
is an integer `b`.  We write a proof harness [harness1.c](examples/harness/harness1.c)

```c
int beta1(int b);

int main() {
   int x;
   beta1(x);
}
```

that allocates an unconstrained integer named `x` and invokes `beta1` on `x`.

We run CBMC on the result with

```bash
goto-cc -o library1.goto library1.c
goto-cc -o harness1.goto harness1.c
goto-cc -o proof1.goto library1.goto harness1.goto
cbmc proof1.goto
```

and we see a violation of the assertion `bb == b`.

```bash
** Results:
library1.c function beta1
[beta1.assertion.1] line 9 assertion bb == b: FAILURE

** 1 of 1 failed (2 iterations)
VERIFICATION FAILED
```

Correcting this assertion to `bb == b + 1` fixes the problem.

## Modeling global state

Sometimes a function depends on global state.  A common example
is low-level system code that depends on a system table, and we want
to know the code works no matter what has been stored in the system
table.

Suppose we have a library [library2.c](examples/harness/library2.c)

```c
#include <assert.h>

int counter = 0;

int alpha2(int a) {
    counter = counter + 1;
    return a+counter;
}

int beta2(int b) {
    int bb = alpha2(b);
    assert(bb == b + 1);
}
```

and we want to test function `beta2`.  This time the function `beta2`
depends on its input `b`, but it also depends on a global variable
`counter` that is initialized to `0` but is changed with each invocation
of `beta2`.  We want to know that `beta2` always works, no matter how
many times `beta2` has been invoked in the past.

We do this by writing a proof harness that chooses an unconstrained value
for both the function input `b` and the global variable `counter`.
We write a proof harness [harness2.c](examples/harness/harness2.c)

```c
extern int counter;
int beta2(int b);

int main() {
    int cnt;
    counter = cnt;

    int x;
    beta2(x);
}
```

We run CBMC on the result with

```bash
goto-cc -o library2.goto library2.c
goto-cc -o harness2.goto harness2.c
goto-cc -o proof2.goto library2.goto harness2.goto
cbmc proof2.goto
```

and see a violation of the assertion `bb == b + 1`.

```bash
** Results:
library2.c function beta2
[beta2.assertion.1] line 12 assertion bb == b + 1: FAILURE

** 1 of 1 failed (2 iterations)
VERIFICATION FAILED
```

We correct this to `bb >= b + 1` and we are done.

## Modeling interfaces

Sometimes a function depends on an implementation of an interface or
a library API that we don't care to test.
A common example is a function that depends on
a network communication protocol like HTTP with `send` and `receive`
methods.  We want our code to work independent of the protocol implementation,
so we replace the implementations of the functions in the protocol interface
with stubs that over-approximate the behaviors of the implementations.

Suppose we have a library [library3.c](examples/harness/library3.c)

```c
#include <assert.h>

void send(int msg);
int receive();

int alpha(int x) {
    int y = receive();
    assert(y > 0);
    return y+1;
}
```

that depends on `send` and `receive` methods from a network interface.
We stub out the network interface with [network3.c](examples/harness/network3.c)

```c
int receive() {
    // model receiving an unconstrained integer value from the network
    int msg;
    return msg;
}

void send(int msg) {
    // model sending an integer over the network (nothing to do)
    return;
}
```

We write the proof harness [harness3.c](examples/harness/harness3.c)

```c
int alpha(int x);

int main() {
    int x;
    alpha(x);
}
```

We run CBMC with

```bash
goto-cc -o network3.goto network3.c
goto-cc -o library3.goto library3.c
goto-cc -o harness3.goto harness3.c
goto-cc -o proof3.goto network3.goto library3.goto harness3.goto
cbmc proof3.goto
```

and we get a failure of the assertion `y > 0`.

Suppose the mistake is in our model of the network.  Suppose our
network only transmits positive integers.  Then we can fix our stub of
the `receive` method to return not an unconstrained integer but an
unconstrained positive integer.
We change our stubs for the network to [network3a.c](examples/harness/network3a.c)

```c
int receive() {
    // model receiving an unconstrained POSITIVE integer value from the network
    int msg;
    __CPROVER_assume(msg > 0);
    return msg;
}

void send(int msg) {
    // model sending an integer over the network (nothing to do)
    return;
}
```

We run CBMC with

```bash
goto-cc -o network3a.goto network3a.c
goto-cc -o library3.goto library3.c
goto-cc -o harness3.goto harness3.c
goto-cc -o proof3.goto network3a.goto library3.goto harness3.goto
cbmc proof3.goto
```

and declare victory at the successful verification.

## Allocating data

For projects where functions take pointers to data on the heap, we
recommend writing functions to allocate unconstrained values on the
heap of the appropriate types for use in proof harnesses.

Consider a string buffer defined by

```c
typedef struct { size_t length; char* buffer; } strbuf;
```

and consider a function that takes a pointer to a string buffer and
caches the fifth character in the buffer into a global variable

```c
char cache;
void cache_fifth_char(strbuf* str) { cache = str->buffer[4]; }
```

Let's try to prove that this function is memory safe (although,
obviously, it is not).  Memory safety means that we never dereference
an invalid pointer and we never try to access data outside the bounds
of a valid object.  We can do this by calling CBMC with the flags
`--pointer-check` and `--bounds-check`.  Let's prove this even in the
case where malloc can fail and return NULL instead of a pointer.  The
default model of malloc in CBMC never fails, but we can change to a
model of malloc that can fail and return NULL by calling CBMC with
the flags `--malloc-may-fail` and `--malloc-fail-null`.

The function `cache_fifth_char` takes a pointer to a string buffer on
the heap, so let write a function that allocates a string buffer.

```c
strbuf* strbuf_allocate(size_t length) {
  strbuf* str = malloc(sizeof(strbuf));
  if (str == NULL) return NULL;
  str->length = length;
  str->buffer = malloc(length);
  return str;
}
```

Now we can write a proof harness for the function

```c
int main() {
  size_t len;
  strbuf* str = strbuf_allocate(len);

  cache_fifth_char(str);
}
```

Pulling all of this together into a single file
[strbuf1.c](examples/harness/strbuf1.c), we can run cbmc on this file
with

```bash
cbmc --pointer-check --bounds-check --malloc-may-fail --malloc-fail-null strbuf1.c
```

and the result is a slew of verification failures including

```bash
[cache_fifth_char.pointer_dereference.1] line 7 dereference failure: pointer NULL in str->buffer: FAILURE
[cache_fifth_char.pointer_dereference.7] line 7 dereference failure: pointer NULL in str->buffer[(signed long int)4]: FAILURE
[cache_fifth_char.pointer_dereference.11] line 7 dereference failure: pointer outside object bounds in str->buffer[(signed long int)4]: FAILURE
```

The first is saying that `str` may be NULL, the second is saying that
`str->buffer` may be NULL, and the third is saying that `str->buffer`
may not have 5 character in it.

We are obviously going to have to make some assumptions about the
string buffer before we can prove that the function is memory safe.

## Validating data

For projects that require allocating objects of various types on the heap, we
recommend writing predicates that say what well-formed objects of these
types look like.

In the case of our example, a valid pointer to a string buffer must be
nonnull, and a valid string buffer must have a buffer that is nonnull.

```c
bool strbuf_is_valid(strbuf* str) {
  if (str == NULL) return false;
  if (str->buffer == NULL) return false;
  return true;
}
```

With this predicate, we can write a proof harness that assumes the string
buffer allocated on the heap is valid before calling the function.  We can
also assert that the string buffer remains valid after calling the function.
And we can assume that the buffer has at least 5 characters in it.
In fact, we can prove the this remains true after the function invocation,
meaning that the function preserves the validity of its input.

```c
int main() {
  size_t len;
  strbuf* str = strbuf_allocate(len);

  __CPROVER_assume(strbuf_is_valid(str));
  __CPROVER_assume(str->length > 4);

  cache_fifth_char(str);

  __CPROVER_assert(strbuf_is_valid(str), "String buffer remains valid");
  __CPROVER_assert(str->length > 4, "String buffer remains length >4");
}
```

Pulling all of this together into [strbuf2.c](examples/harness/strbuf2.c),
we can run CBMC

```bash
cbmc --pointer-check --bounds-check --malloc-may-fail --malloc-fail-null strbuf2.c
```

and rejoice in the successful validation.

## Proof assumptions

Notice that in our last proof harness, we worked hard to make as
explicit as possible all of the assumptions required for memory safety
to hold: The pointer must be a valid pointer to a valid string buffer
with at least five characters in it. It is the responsibility of the
programmer to make certain that these assumptions are true when the
function is called.
All bets are off when the function is called in a weaker context where some of these assumptions are false. The memory safety proof of the function does not hold in that context since the assumptions it is based on do not hold.
