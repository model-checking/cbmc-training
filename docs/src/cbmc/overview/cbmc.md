# Running cbmc

At this point we already have some experience with running CBMC
on simple source files.  For example, we have used CBMC to
find [memory safety issues](debugging.html#memory-safety)
and debug a [termination issue](debugging.html#termination).

One thing we have touhed on only lightly is how to interpret the
output of CBMC.  Let's do that now.

Consider the following program [vector.c](examples/vector.c)
```c
#include <stdlib.h>
#define LENGTH 2

void init(int *, size_t);

int main() {
  int vector[LENGTH];

  init(vector, LENGTH);
  for (int i=0; i<=LENGTH; i++)
    vector[i] = vector[i] + 1;

  return 0;
}
```

Suppose we run CBMC on this program and ask CBMC to do array bounds checking
and to produce an error trace for every array bounds violation that it finds.
```bash
cbmc vector.c --bounds-check --trace
```

Let's examine the output of CBMC one section at a time.
After some preliminary output, CBMC begins with
```
Starting Bounded Model Checking
**** WARNING: no body for function init
Unwinding loop main.0 iteration 1 file vector.c line 13 function main thread 0
Unwinding loop main.0 iteration 2 file vector.c line 13 function main thread 0
Unwinding loop main.0 iteration 3 file vector.c line 13 function main thread 0
```

CBMC is warning us that it found no function body for `init`.
We included the function declaration but forgot to include the function
definition.  CBMC treats a function like `init` with no function body as
a function with no side effects (`init` will not initialize the vector)
that returns an unconstrained return value (`init` will return an
arbitrary integer value).

CBMC is also telling us that it unwounded the for loop three times.
This makes sense since the array has `LENGTH=2`.  Notice that CBMC
gave this first (and only) loop in the function `main` the loop name `main.0`.

After some logging output (describing the sequence of transformations
CBMC performs on the program to turn it into a Boolean constraint problem
and invocation of the SAT solver to solve the constraint problem),
CBMC prints its results.

```
** Results:
vector.c function main
[main.array_bounds.1] line 14 array 'vector' lower bound in vector[(signed long int)i]: SUCCESS
[main.array_bounds.2] line 14 array 'vector' upper bound in vector[(signed long int)i]: FAILURE
```

CBMC is reporting that it performed two array bounds check on line 14:
it checked whether the lower bound of `vector` could be violated, and whether
the upper bound could be violated. The lower bound check succeded and the
upper bound check failed.

Finally, CBMC prints a complete error trace for each check that failed.
In our case, it was the second array bounds check in the function `main`
that failed, and CBMC gave that check the name `main.array_bounds.2`.
```
Trace for main.array_bounds.2:

State 21 file vector.c function main line 10 thread 0
----------------------------------------------------
  vector={ 2760991, 33628711 } ({ 00000000 00101010 00100001 00011111, 00000010 00000001 00100010 00100111 })

State 26 file vector.c function main line 13 thread 0
----------------------------------------------------
  i=0 (00000000 00000000 00000000 00000000)

State 28 file vector.c function main line 14 thread 0
----------------------------------------------------
  vector[0l]=2760992 (00000000 00101010 00100001 00100000)

State 29 file vector.c function main line 13 thread 0
----------------------------------------------------
  i=1 (00000000 00000000 00000000 00000001)

State 32 file vector.c function main line 14 thread 0
----------------------------------------------------
  vector[1l]=33628712 (00000010 00000001 00100010 00101000)

State 33 file vector.c function main line 13 thread 0
----------------------------------------------------
  i=2 (00000000 00000000 00000000 00000010)

Violated property:
  file vector.c function main line 14 thread 0
  array 'vector' upper bound in vector[(signed long int)i]
  !((signed long int)i >= 2l)

```
The trace is a step-by-step execution of the program from an initial
state to the array bounds violation.  The trace has six steps numbered
21 through 33 because there were a lot of steps internal to CBMC that
it did not include in the trace.  The steps included in the trace
are assignments to program variables made during the execution.

The first step shows the initialization of `vector`.  CBMC chose
two fairly large integers as the initial values of the two
integers in the array.  Notice that CBMC is printing the values in
both decimal and binary notation.

The next four steps show the variable assignments made during
the first two iterations of the loop: each iteration does
one assignment to `i` and one assignment to `vector`.

The final two steps show the assignemnt `i=2` and the attempt to
access `vector[i]` with `i==2` that caused the array bounds violation.
