# What is loop unwinding?

CBMC is a bounded model checker.  CBMC works by
executing a program for some bounded number of steps and looking
for issues that occur within those steps.  CBMC does not execute a
program concretely in the same way that a computer would execute the program
(would execute the result of compiling the program).
Instead,
CBMC [symbolically executes](https://en.wikipedia.org/wiki/Symbolic_execution)
the program to turn it into a set of symbolic constraints that can be solved by
a constraint solver like a SAT solver.

But where does this bound come from?  And how do we know the bound
is the right one?

## Loop bounds

If the program contains no loops and contains no recursion, then every
execution of the program is finite, and CBMC can figure out the length
of the longest execution and use this as the bound.  Sometime CBMC
can figure this out even when the program contains loops.  For example,
CBMC can loop at the loop
```
  for (int i=0; i < 10; i++)
    buffer[i] = ' ';
```
and figure out on its own that the loop iterates at most 10 times.
But CBMC needs some help with the loop
```
  while (TRUE)
    if (i < buffer.size) buffer.data[i] = ' ' else break;
```
We need to give CBMC a bound on the number of times this loop iterates.
We need to tell CBMC how many times to unwind this loop during its
symbolic execution of the program.

We can do this in two ways.

* **Bound all loops**: We can instruct CBMC to unwind every loop at
  most `K` times with `cbmc --unwind K`. If you are using the CBMC
  starter kit, this bound 1 by default, but you can change this
  to `K` in your Makefile with
  ```
    UNWIND=K
  ```

* **Bound one loop**: We can instruct CBMC to unwind a specific loop
  at most `K` times with `cbmc --unwindset FUNC.NUM:K` where `FUNC`
  is the name of the function containing the loop and `NUM` is the
  number of the loop within the function (more on loop numbers below).
  If you are using the CBMC starter kit, we recommend that you set
  this bound in your Makefile with
  ```
     UNWINDSET+=FUNC.NUM:K
  ```

## Loop unwinding assertions

Now we know how to tell CBMC to unwind a loop `K` times, but how do we
know that unwinding a loop `K` times is enough?  What if the problem
we are looking for occurs on iteration `K+1` and we have asked CBMC to
unwind the loop only `K` times?  We could miss the issue and never know
it because we didn't tell CBMC to work hard enough to find it!

CBMC has solution called "loop unwinding assertions."
A loop unwinding assertion is a check that a loop has been completely unwound.
It checks that the loop termination condition is guaranteed to be true
after the specified number of unwindings.
First CBMC unwinds the loop, and then CBMC inserts a assertion that
the loop termination condition is true immediately after the final
unwinding of the loop.
If there is any execution of the program in which you haven't unwound
the loop enough times, CBMC will identify the loop by its
loop name `FUNC.NUM` in the list of checks that have failed.

Invoking CBMC with `cbmc --unwinding-assertions` will ask CBMC to check
unwinding assertions.

<center>Always invoke CBMC with <code>cbmc --unwinding-assertions</code>.</center>

You can rest assured that CBMC is always invoked
with `cbmc --unwinding-assertions` if you are using the CBMC starter kit.

## Counting loop iterations

There are two ways to count loop iterations:

* One is the number of times `B` that the loop body is traversed.
* One is the number of times `T` that the loop termination is checked.

These two numbers differ by one: `T = B + 1`.

When you invoke CBMC, the number `K` that you give to `--unwind` or
`--unwindset` is interpreted as the number of times the loop termination
is checked: `K = T = B+1`.

This off-by-one business sometimes confuses people, and sometimes makes
it challenging to write a Makefile where arithmetic like `B+1` is difficult.
One style we have developed is to write a proof harness

```C
int main() {
  size_t length;
  __CPROVER_assume(length < LENGTH);

  char *buffer = (char *) malloc(length);
  for (int i=0; i < length; i++) buffer[i] = ' ';
  ...
}
```

and to write a Makefile

```Makefile
LENGTH=100
DEFINES += -DLENGTH=$(LENGTH)
UNWINDSET += main.0:$(LENGTH)
```

Because `length` is assumed to be strictly less than `LENGTH`, we know
that `length + 1` is at most `LENGTH`, so unwinding the loop `main.0`
at most `LENGTH` times is unwinding the loop at least `length + 1` times
as desired.

A final comment on this off-by-one issue:
At some point in the future, you may come to learn that
you can also pass the `cbmc` flags `--unwind` and `--unwindset` to a tool
named `goto-instrument`.  For historical reasons, `cbmc` interprets `K` as
`K=T` and `goto-instrument` interprets `K` as `K=B`.
Of course, when choosing between unrolling a loop `T=B+1` and `B` times,
it is always safe to unroll a loop more times than necessary.
But it may not be practical: If the loop body is
large and you are already unrolling a loop many times, the difference between
unrolling `K` and `K+1` times many push CBMC execution time from "okay"
to "way too long."
Just something to keep in mind when using these tools.

## Debugging loop unwinding

When not using the starter kit, which uses `--unwind 1` by default, you may find
yourself in a situation where CBMC keeps unwinding a loop, even when you
expected CBMC to be able to determine the loop bound. CBMC uses constant
propagation and algebraic simplification to evaluate the loop guard.  CBMC will
stop unwinding the loop when, using this process, the loop guard evaluates to
false. If you would like to debug a case of unexpected loop unwinding, proceed
as described below. We will use the following example to illustrate these steps:

```C
int main()
{
  int step;
  for(int i = 0; i < 5; i += step) ;
}
```

Note that `step` is unconstrained, perhaps unintentionally so.
Running CBMC on this example without any additional options yields:

```
> cbmc loop.c
[...]
Starting Bounded Model Checking
Unwinding loop main.0 iteration 1 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 2 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 3 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 4 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 5 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 6 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 7 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 8 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 9 file loop.c line 4 function main thread 0
Unwinding loop main.0 iteration 10 file loop.c line 4 function main thread 0
[...]
```
until you manually abort this process. One way to understand why this is
happening is to undertake the following steps:

1. Run CBMC with whatever options you normally do, but add some finite unrolling
   bound using `--unwind` and also add `--program-only`. This will yield as
   output the equation system generate by symbolic execution. Pipe the output to
   some text file. We exemplify this on the above example:
   ```
   > cbmc loop.c --program-only --unwind 2
   [...]
   Starting Bounded Model Checking
   Unwinding loop main.0 iteration 1 file loop.c line 4 function main thread 0
   Not unwinding loop main.0 iteration 2 file loop.c line 4 function main thread 0
   [...]
   Program constraints:
   [...]
   // 2 file loop.c line 4 function main
   (26) i!0@1#2 == 0
   // 3 file loop.c line 4 function main
   // 4 file loop.c line 4 function main
   (27) i!0@1#3 == step!0@1#1
   // 5 file loop.c line 4 function main
   // 3 file loop.c line 4 function main
   // 3 file loop.c line 4 function main
   (28) \guard#1 == !(i!0@1#3 >= 5)
   // 4 file loop.c line 4 function main
   (29) i!0@1#4 == i!0@1#3 + step!0@1#1
        guard: \guard#1
   [...]
   ```
   The above output is an excerpt from the equation system generated by symbolic
   execution. We see assignments to the loop counter `i` turned into equations
   over its renamed (in static single assignment (SSA) form) instantiations. The
   loop guard is found as `\guard#1`.
2. Search for the source location where you expect the constant to appear in the
   loop guard. In our example, this is "file loop.c line 4."
3. Once you have found the assignment or equality defining the loop guard that
   doesn't have the expected constant on the right-hand side (in the example,
   this could be the equality over `\guard#1`), work backwards
   from that right-hand side to see where you last had a constant. That is,
   backwards-search for the name appearing on the right-hand side, which will
   eventually appear as a left-hand side. Now repeat this process for the new
   right-hand side of this equation, until the right-hand side is a constant. In
   the example, the only time we find a constant as right-hand side is the
   initial assignment, `i!0@1#2 == 0`.
4. The expression where you flip from is-a-constant to is-a-symbol is the
   culprit. For our example, this is `i!0@1#3 == step!0@1#1`. You will then
   conclude that either this behavior is as expected, meaning your source
   program does not permit constant propagation (our example does not permit
   constant propagation, because `step` is unconstrained; setting it to some
   constant value will result in bounded loop unwinding), or a shortcoming in
   CBMC's simplification process. In the latter case, report an issue or
   contribute a patch to address this.
