# CBMC as unit testing

Think of CBMC as a form of unit testing: If you can write a unit test,
then you can write a CBMC proof.  One of the things we can do with
unit testing is to run a function on a set of inputs, and to compare
the result with the expected result.  Think of CBMC as a unit tester
that runs the function on all possible inputs.

Here is a
function `quartile` from [quartile.c](quartile.c)
that returns the quartile of an integer between 0 and 99.
```c
int quartile(int x) {
  int y;

  if (x < 0 || x > 99) return 0;

  if (x < 50)
    if (x < 25)
      y = 1;
    else
      y = 2;
  else
    if (x < 75)
      y = 3;
    else
      y = 4;

  return y;
}
```

Here a unit test from [unit-test.c](unit-test.c) that tests
`quartile` using a number from the first quartile.
```c
#include <assert.h>
int quartile(int x);

int main() {
  int x = 1;
  int result = quartile(x);
  assert(result == 1);
}
```

We can build the unit test with `gcc` and run it.
When we do, the unit test runs successfully,
and the assertion does not fail:
```bash
gcc quartile.c unit-test.c -o unit-test
./unit-test
```

We can also build the unit test with `goto-cc`, which is a drop-in
replacement for `gcc` that comes with CBMC.  Now we can use CBMC to
run the unit test, and again the assertion does not fail:
```bash
goto-cc quartile.c unit-test.c -o unit-test
cbmc ./unit-test
```
```
** Results:
unit-test.c function main
[main.assertion.1] line 8 assertion result == 1: SUCCESS

** 0 of 1 failed (1 iterations)
VERIFICATION SUCCESSFUL
```

For examples as simple as this,
we can actually run CBMC directly on the source files
(without explicitly invoking `goto-cc`):
```
cbmc quartile.c unit-test.c
```
```
** Results:
unit-test.c function main
[main.assertion.1] line 8 assertion result == 1: SUCCESS

** 0 of 1 failed (1 iterations)
VERIFICATION SUCCESSFUL
```

But now we can do something interesting with CBMC.
The unit test currently initializes the variable `x` to 1,
and CBMC considers 1 to be the initial value of `x`.  If
we remove the initializer, CBMC allows any integer value to be
the initial value of `x`. This is a general rule: If CBMC encounters
an uninitialized variable, then CBMC allows any value from the variable's
type to be the variable's initial value.
We say that CBMC treats uninitialized variables as having unconstrained
initial values.

If we rewrite the unit test
```c
#include <assert.h>
int quartile(int x);

int main() {
  int x; // The value of x is now any value of type int
  int result = quartile(x);
  assert(result == 1);
}
```
and rerun CBMC
```bash
cbmc quartile.c unit-test.c
```
```
** Results:
unit-test.c function main
[main.assertion.1] line 8 assertion result == 1: FAILURE

** 1 of 1 failed (2 iterations)
VERIFICATION FAILED
```
we can see that the assertion has failed.  We can ask CBMC to produce
an error trace that demonstrates one way in which the assertion can fail

```bash
cbmc quartile.c unit-test.c --trace
```
```
** Results:
unit-test.c function main
[main.assertion.1] line 8 assertion result == 1: FAILURE

Trace for main.assertion.1:

State 21 file unit-test.c function main line 6 thread 0
----------------------------------------------------
  x=-2147483547 (10000000 00000000 00000000 01100101)

State 22 file unit-test.c function main line 7 thread 0
----------------------------------------------------
  result=0 (00000000 00000000 00000000 00000000)

State 26 file unit-test.c function main line 7 thread 0
----------------------------------------------------
  x=-2147483547 (10000000 00000000 00000000 01100101)

State 27 file quartile.c function quartile line 2 thread 0
----------------------------------------------------
  y=0 (00000000 00000000 00000000 00000000)

State 33 file unit-test.c function main line 7 thread 0
----------------------------------------------------
  result=0 (00000000 00000000 00000000 00000000)

Violated property:
  file unit-test.c function main line 8 thread 0
  assertion result == 1
  !((signed long int)(signed long int)!(result == 1) != 0l)

** 1 of 1 failed (2 iterations)
VERIFICATION FAILED
```
and we see that in the first step of this error trace the variable `x`
is initialized to the integer value `x=-2147483547`.

Well, that's not quite what we intended, but we don't expect `quartile`
to return 1 on every integer, just on integers in the first quartile.
We can constrain `x` to be an integer in the first quartile by adding
an assumption to the unit test:
```c
#include <assert.h>
int quartile(int x);

int main() {
  int x;
  __CPROVER_assume(0 <= x && x < 25);    // this is a precondition
  int result = quartile(x);
  assert(result == 1);                   // this is a postcondition
}
```

Rerunning CBMC, we see that the assertion is always true:
```bash
cbmc quartile.c unit-test.c
```
```
** Results:
unit-test.c function main
[main.assertion.1] line 9 assertion result == 1: SUCCESS

** 0 of 1 failed (1 iterations)
VERIFICATION SUCCESSFUL
```

Let's pause for a moment to notice that we have already done something
interesting.
We have proved that the function `quartile` returns 1 when it is called
on any integer in the first quartile.
We have partially specified the behavior of the function using
a precondition (the assumption that `x` is in the first quartile)
and a postcondition (the assertion that the return value is `1`).
If the function is called with an input that satisfies the precondition,
then the function returns with a value that satisfies the postcondition.
Of course, we have only partially specified the behavior of the function.
What happens when it is called with an integer in the second
quartile?  The specification doesn't say.  But we have taken
the first step toward specification and verification of our function.
