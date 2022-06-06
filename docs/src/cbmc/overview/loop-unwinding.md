# Loop unwinding

CBMC is a *bounded* model checker for C.
CBMC works by unwinding the loops in our code
(by inlining a finite number of iterations of the loop).
CBMC needs to know a bound on the number of times it is expected to
unwind any particular loop in our code.
In the same way, CBMC needs to know a bound on the number of times
to invoke any recursive function in our code.

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

and CBMC will unwind the first loop until it runs out of resources
or the loop index wraps around.
We need to tell CBMC how
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
unwinding to 11 iterations.  We can use the flag `--unwindset` to give
different bounds to different loops.  CBMC names the loops in a
program with the name of the function containing the loop and the
position of the loop within the function.  We can ask CBMC to
display loop names with the `--show-loops` flag
```
cbmc --show-loops loop2.c
```
and see that the two loops in `loop2.c` are named `main.0` and `main.1`.
We can run CBMC with
```
cbmc --unwindset main.0:11 --unwindset main.1:11 loop2.c
```
and get the same successful verification result as with `--unwind 11`.
The flags `--unwind` and
`--unwindset` can be used together and with different bounds.
CBMC will use the `--unwind` bound
by default and use the `--unwindset` bound for the loop it names.

## Loop unwinding assertions

Now, however, we have a problem.  CBMC has proved that there is no assertion
failure in the program as long as we limit loops to 10 iterations.  But
how do we know that 10 iterations is enough?  What if it is possible for a
loop to iterate 11 times, and the error we are looking for occurs on the
11th iteration, and we missed it?

CBMC has a solution for this called *loop unwinding assertions*.
We can ask CBMC to insert loop unwinding assertions with the flag
`--unwinding-assertions`.
Now, after CBMC has unrolled a loop, it will insert immediately after the
unrolled loop an assertion that the loop termination is true.  In other words,
CBMC will check that it has completely unrolled the loop.
For example, running
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
