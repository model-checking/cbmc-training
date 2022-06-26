# CBMC as debugging

Think of CBMC as a form of debugging: CBMC does an exhaustive search for
issues affecting functional correctness and memory safety.  Let's look
at a few examples of the kinds of things that CBMC can find.

## Memory safety

The source code in [memory-safety.c](examples/simple/memory-safety.c)
```c
#define SIZE 20
char buffer[SIZE];

char read_buffer(int i)  { return buffer[i];     }
char read_pointer(int i) { return *(buffer + i); }

int main() {
  int index;
  read_buffer(index);
  read_pointer(index);
}
```
allocates a buffer of 20 characters and defines two functions that return
a character from the buffer.  The first function `read_buffer` uses array
indexing to access the character.  The second function `read_pointer` uses
pointer dereferencing.  Neither function does any bounds checking.
The `main` function declares an integer-valued
`index` and invokes the two functions to access the character at this index.
The fact that index
is uninitialized is significant to CBMC: When CBMC sees an uninitialized
variable, it considers as possible values for that variable all values a
variable of that type can assume.

If you ask CBMC to check buffer accesses,
```bash
cbmc memory-safety.c --bounds-check
```
CBMC reports violations of both
the lower and upper bounds of the array (because `index` can assume both
negative and positive values):
```
line 3 array 'buffer' lower bound in buffer[(signed long int)i]: FAILURE
line 3 array 'buffer' upper bound in buffer[(signed long int)i]: FAILURE
```

If you ask CBMC to check pointer dereferencing,
```bash
cbmc memory-safety.c --pointer-check
```
CBMC reports that the pointer
can point outside the bounds of the buffer:
```
line 4 dereference failure:
  pointer outside object bounds in buffer[(signed long int)i]: FAILURE
```

If you ask CBMC to produce an error trace,
```bash
cbmc memory-safety.c --pointer-check  --trace
```
you get a step-by-step execution of the program leading to the error, and
the trace ends with the following steps:
```
State 42 file memory-safety.c function main line 7 thread 0
----------------------------------------------------
  index=21 (00000000 00000000 00000000 00010101)

State 45 file memory-safety.c function main line 8 thread 0
----------------------------------------------------
  i=21 (00000000 00000000 00000000 00010101)

State 49 file memory-safety.c function main line 9 thread 0
----------------------------------------------------
  i=21 (00000000 00000000 00000000 00010101)

Violated property:
  file memory-safety.c function read_pointer line 4 thread 0
  dereference failure: pointer outside object bounds in buffer[(signed long int)i]
  (signed long int)i >= 0l && !((unsigned long int)(signed long int)i + 1ul >= 21ul)
```
CBMC is saying in step 42 that if you choose the value 21 for the `index`,
then `*(buffer + index)` points to a location outside the 20-character
buffer.

## Termination

The source code in [termination.c](examples/simple/termination.c)
```c
#include <stdlib.h>
int main() {
  size_t initial_size;
  size_t requested_size;

  size_t size = initial_size;
  while (size < requested_size) {
    size *= 2;
  }
}
```
illustrates a bug found with CBMC that actually required some
unexpected code changes.
This example reduces the problem to a single loop from an allocator,
and makes the mistake more obvious than it was in the original code.
The allocator begins with an initial
(recommended) size for chunk allocation,
and repeatedly doubles this size until it is large
enough to hold the requested size.  Each iteration of the loop doubles
the size (shifts the size left by one bit) until the size overflows
to 0.  Once the size is 0, doubling the size results in 0, and the loop
never ends.

How would you find this with CBMC (without already knowing the problem exists)?
If you run simply CBMC with
```bash
cbmc termination.c
```
CBMC will go on forever unwinding the loop forever because it cannot easily
tell from the source code how many times the loop iterates.

You can coach CBMC to unwind the loop just four times with the `--unwind` flag
```bash
cbmc termination.c --unwind 4
```
and CBMC terminates with apparent success:
```
VERIFICATION SUCCESSFUL
```
But how do you know that unwinding the loop 4 times is enough?  What if
something bad happens on the 5th iteration and you just never asked
CBMC to look beyond the 4th iteration?

CBMC has a solution. We always run CBMC with an extra flag
```bash
cbmc termination.c --unwind 4  --unwinding-assertions
```
that checks that loops have been completely unwound: It checks
that the loop termination condition is true after the
loop unwinding.  If you run CBMC with this flag, CBMC reports failure
(the loop unwinding assertion has failed):
```
line 7 unwinding assertion loop 0: FAILURE
```

Try unwinding 40 times:
```c
cbmc termination.c --unwind 40  --unwinding-assertions
```
```
line 7 unwinding assertion loop 0: FAILURE
```

Try unwinding 80 times:
```c
cbmc termination.c --unwind 80  --unwinding-assertions
```
```
line 7 unwinding assertion loop 0: FAILURE
```

Try asking for a trace:
```
cbmc termination.c --unwind 80  --unwinding-assertions --trace
```
Now you see that `size` is stuck at 0.  The final steps of
the trace are
```
State 257 file termination.c function main line 8 thread 0
----------------------------------------------------
  size=0ul (00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000)

State 260 file termination.c function main line 8 thread 0
----------------------------------------------------
  size=0ul (00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000)

State 263 file termination.c function main line 8 thread 0
----------------------------------------------------
  size=0ul (00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000)

Violated property:
  file termination.c function main line 7 thread 0
  unwinding assertion loop 0

```

Try unwinding 64 times since you probably have a 64-bit machine:
```
cbmc termination.c --unwind 64 --unwinding-assertions --trace
```
In the final two steps of the trace you can see the overflow
from a large integer to 0:
```
State 212 file termination.c function main line 8 thread 0
----------------------------------------------------
  size=9223372036854775808ul (10000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000)

State 215 file termination.c function main line 8 thread 0
----------------------------------------------------
  size=0ul (00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000)

Violated property:
  file termination.c function main line 7 thread 0
  unwinding assertion loop 0



** 1 of 1 failed (2 iterations)
VERIFICATION FAILED
```
