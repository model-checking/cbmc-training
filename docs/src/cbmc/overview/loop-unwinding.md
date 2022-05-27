# Loop unwinding

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

In summary, we use the flag `--unwind` to tell CBMC to bound the
unwinding of loops in the program.  We use `--unwind 11` to bound loop
unwinding to 11 iterations.  We can use the flag `--unwindset` to give a
different bound to a particular loop.  CBMC gives names to loops in a
program by giving the name of the function containing the loop and the
number of the loop within the function.  For example, the two loops in the
program above would be named `main.0` and `main.1`.  We could tell CBMC to
unwind all loops in the program at most 11 times, but to unwind the
second loop in main at most 21 times.  We do this by invoking CBMC with
```
cbmc --unwind 11 --unwindset main.1:21 loop2.c
```
and we get the same successful verification result.



## Loop uwinding assertions

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
