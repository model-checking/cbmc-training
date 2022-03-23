# Checking assertions

This section gives simple examples of running CBMC on simple programs
to prove assertions within those programs.
Perhaps most important, this section introduces
the concept of proof assumptions, and the importance of understanding the
assumptions that a proof is making.

* [Running CBMC](#running-cbmc)
* [Unrolling loops](#unrolling-loops)
* [Unrolling loops soundly](#unrolling-loops-soundly)
* [Proof assumptions](#proof-assumptions)

## Running CBMC

If we run CBMC on the following program from [int1.c](examples/assertions/int1.c)
```c
#include <assert.h>

int main() {
  int x;
  assert(x == 0);
  return 0;
}
```
with
```bash
cbmc int1.c
```
we will see a report of an assertion failure

```bash
[main.assertion.1] line 5 assertion x == 0: FAILURE
VERIFICATION FAILED
```

CBMC considers uninitialized variables to have unconstrained values.
When CBMC examines this program, it finds an uninitialized variable `x`
of type `int`.  The type `int` describes a set of integer values that
a variable of type `int` can assume.  CBMC considers all executions
of the program in which `x` assumes all values of type `int`.  At least
one of those values is not 0, and in that execution of the program,
the assertion fails.

We can see what value CBMC discovered for `x`
by asking CBMC to produce an error trace whenever it finds
an assertion failure.  We do this by running CBMC with
the flag `--trace`. The trace describes one step-by-step
path through the code that leads to the assertion failure.  For
example, the output of the command

```bash
cbmc --trace int1.c
```

consists of a single step

```bash
State 24 file int1.c function main line 4 thread 0
----------------------------------------------------
  x=67108864 (00000100 00000000 00000000 00000000)
```
that initializes `x` to `67108864` (the value in your counterexample
trace may differ).  If we initialize `x` to `0` as in the program
[int2.c](examples/assertions/int2.c)

```c
#include <assert.h>

int main() {
  int x = 0;
  assert(x == 0);
  return 0;
}
```

and run CBMC with the command

```bash
cbmc int2.c
```

then CBMC reports that the verification is successful.

NOTE: The first state in the trace printed above
is numbered 24 because CBMC runs through
a number of its own internal steps before getting to the
our code.  CBMC does not print states resulting from its
own internal steps, only states corresponding to variable
assignments in our own code.

## Unrolling loops

CBMC is a *bounded* model checker for C.  CBMC works
by unwinding the loops in our code (ie by unrolling/inlining a finite number of iterations of the loop). CBMC needs to know a bound on the number of times it is expected to unwind any particular
loop in our code.

Sometimes CBMC can figure this out on its own.
Consider the program [loop1.c](examples/assertions/loop1.c)

```c
#include<assert.h>

int main() {
  unsigned array[10];

  for (int i = 0; i < 10; i++) {
    array[i] = 0;
  }

  for (int i = 0; i < 10; i++) {
    assert(array[i] == 0);
  }
  return 0;
}
```

and run CBMC with

```bash
cbmc loop1.c
```

and CBMC will print

```bash
[main.assertion.1] line 11 assertion array[i] == 0: SUCCESS
VERIFICATION SUCCESSFUL
```

Sometimes CBMC needs help.  Consider the program [loop2.c](examples/assertions/loop2.c)

```c
#include<assert.h>

int main() {
  unsigned bound;
  unsigned array[bound];

  for (int i = 0; i < bound; i++) {
    array[i] = 0;
  }

  for (int i = 0; i < bound; i++) {
    assert(array[i] == 0);
  }
  return 0;
}
```

and run CBMC with

```bash
cbmc loop2.c
```

and CBMC will unwind the first loop forever.  We need to tell CBMC how
many times to unwind the loops in the program.
Run CBMC with

```bash
cbmc --unwind 11 loop2.c
```

and CBMC will unwind every loop in the program 10 times (not 11 times!) and
print

```bash
[main.assertion.1] line 11 assertion array[i] == 0: SUCCESS
VERIFICATION SUCCESSFUL
```

## Unrolling loops soundly

Now, however, we have a problem.  CBMC has proved that there is no assertion
failure in the program as long as we limit loops to 10 iterations.  But
how do we know that 10 iterations is enough?  What if it is possible for a
loop to iterate 11 times, and the error we are looking for occurs on the
11th iteration?

We can ask CBMC to prove that we have unwounded loops enough times: We
can ask CBMC to prove that the loop termination condition is guaranteed
to be true after the specified number of unwindings.  We do this by
running CBMC with the flag `--unwinding-assertions`.  For example, running
CBMC on the first program [loop1.c](examples/assertions/loop1.c) with

```bash
cbmc --unwind 11 --unwinding-assertions loop1.c
```

succeeds with

```bash
VERIFICATION SUCCESSFUL
```

but running CBMC on the second program [loop2.c](examples/assertions/loop2.c) with

```bash
cbmc --unwind 11 --unwinding-assertions loop2.c
```

fails with output that includes

```bash
[main.unwind.0] line 7 unwinding assertion loop 0: FAILURE
VERIFICATION FAILED
```

## Proof assumptions

In hindsight, the problem is obvious: If the initialized variable `bound`
happens to begin with the value `11`, then unwinding loops only 10 times will
not be enough.
If we are only going to unwind loops 10 times, then we have to restrict
ourselves to arrays of at most 10 elements.
We need to tell CBMC to assume that `bound` is at most 10.
We make this assumption with the instrinsic function `__CPROVER_assume`.
Consider the program [loop2a.c](examples/assertions/loop2a.c)

```c
#include<assert.h>

int main() {
  unsigned bound;
  unsigned array[bound];

  __CPROVER_assume(bound < 11);

  for (int i = 0; i < bound; i++) {
    array[i] = 0;
  }

  for (int i = 0; i < bound; i++) {
    assert(array[i] == 0);
  }
  return 0;
}
```

and run CBMC with the command

```bash
cbmc --unwind 11 --unwinding-assertions loop2a.c
```

and CBMC reports success.

We now come, however, to the harsh reality of mathematical proof:

* Every proof makes assumptions, and the quality of the proof depends on the quality of the assumptions.

If the proof assumptions are true in practice,
then the proof is valid in practice.
If the assumptions are only sometimes true, however,
then the proof is valid only in those cases.

When we write a proof for a function, we generally make three classes of
assumptions:

* we may bound the size of input to the function with explicit assumptions
  like `bound < 11`,
* we may model the environment of the function with a proof harness that
  we will describe in the next section, and
* we may stub out some of the functions invoked by the function under test.

When reading or writing a proof, it is essential to pay attention to
the assumptions being made, and to make these assumptions as explicit
and easy-to-find as possible.
