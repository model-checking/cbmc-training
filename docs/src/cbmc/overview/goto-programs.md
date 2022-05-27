# Goto programs

The goto program is the intermediate representation of a program used by
CBMC to do bounded model checking.
CBMC comes with a compiler called `goto-cc` that is intended to be a
drop-in replacement for `gcc`.  The difference is that `goto-cc` produces
a goto program instead of the executable binary produced by `gcc`.
We've already [touched on these topics](proof.md#building-code-for-proof)
in our [overview](proof.md) of how to use CBMC.
Most important, we've demonstrated how to use `goto-cc` to produce the
goto program and how to run CBMC on the goto program.

Sometimes, however, it is helpful (and just plain interesting) to look at
the goto program produced by `goto-cc`.  Two commands are particularly
helpful:

* `cbmc program.goto --list-goto-functions` displays the function names of
  all the functions appearing in the goto program `program.goto`
* `cbmc program.goto --show-goto-functions` displays function bodies of all
  the functions appearing in the goto program `program.goto`.

## A goto program

Consider a little program [function-pointer.c](examples/function-pointer.c)
that declares two functions `alpha` and `beta`,
assigns nondeterministically
one of `alpha` and `beta` to a function pointer `function`,
and then invokes the function pointed to by `function`.
```c
#include <assert.h>

int alpha(int x) {
  return x+1;
}

int beta(int x) {
  return x+2;
}

main() {
  int (*function)();

  int bool;
  function = bool ? alpha : beta;

  int x;
  int rc = function(x);
}
```

Let's compile the program into a goto program.
```bash
goto-cc function-pointer.c -o function-pointer.goto
```

## The function names

Let's print the function names:
```bash
cbmc function-pointer.goto --list-goto-functions
```
```
__CPROVER__start /* __CPROVER__start */
__CPROVER_initialize /* __CPROVER_initialize */
alpha /* alpha */
beta /* beta */
main /* main */
```

The second `initialize` function initializes some CBMC internal variables.
The first `start` function is the entry point for the goto program.
It calls the `initialize` function followed by the `main` function.
You can print the function bodies and see this for yourself.

## The function bodies

Let's print the function bodies:
```bash
cbmc function-pointer.goto --show-goto-functions
```

The result is a little too long to include here, but in the middle
of the function body for `main` we find
```
     // 7 file function-pointers.c line 18 function main
     IF main::1::function = cast(address_of(beta), code*) THEN GOTO 1
     // 8 file function-pointers.c line 18 function main
     IF main::1::function = cast(address_of(alpha), code*) THEN GOTO 2
     // 9 file function-pointers.c line 18 function main
     ASSUME false
     // 10 file function-pointers.c line 18 function main
  1: CALL main::$tmp::return_value := beta(main::1::x)
     // 11 file function-pointers.c line 18 function main
     GOTO 3
     // 12 file function-pointers.c line 18 function main
  2: CALL main::$tmp::return_value := alpha(main::1::x)
     // 13 no location
  3: ASSIGN main::1::rc := main::$tmp::return_value
```

This is interesting because it demonstrates how CBMC handles
function pointers.  What we see is essentially a switch statement
that compares the function pointer `function` with pointers to
each of the functions whose address has been taken anywhere in the program.
In this case, pointers to the functions `alpha` and `beta`.  Depending on
the outcome of the comparison, either `alpha` or `beta` is invoked
and its return value is assigned to `rc`.

We include this discussion as an example of how looking at the goto program
can help debug issues with CBMC.  If, for example, CBMC were surprisingly
slow to terminate, you might look at the goto program and discover that
there is another function `gamma`
that is being considered as a possible value for `function`.
This is impossible in our example, but if `gamma` is defined in another
source file that was accidentally included in the build of the goto program,
one solution might be simply to omit that source file from the build for
this proof.
