# Property checking

CBMC can check more than just assertions in the code.
In this section, we describe properties like memory safety that we
check for every time we run CBMC.
For the full list of things that CBMC can check, run `cbmc --help`.

* [Memory safety](#memory-safety)
* [More pointer checks](#more-pointer-checks)
* [Malloc failure](#malloc-failure)
* [Integer overflow](#integer-overflow)
* [Floating point overflow](#floating-point-overflow)
* [Division by zero](#division-by-zero)
* [Type casting](#type-casting)
* [Bit shifting](#bit-shifting)
* [Loop unwinding](#loop-unwinding)


## Memory safety

* `--bounds-check` enables array bounds checking.
* `--pointer-check` enables invalid pointer checking.

Given the program [`bounds.c`](examples/properties/bounds.c)
```c
main() {
  int array[10];
  for (int i=0; i<=10; i++) array[i] = i;
}
```
the command
```bash
cbmc --bounds-check bounds.c
```
finds the off-by-one error in the loop termination condition:
```
line 3 array 'array' lower bound in array[(signed long int)i]: SUCCESS
line 3 array 'array' upper bound in array[(signed long int)i]: FAILURE
```

Given the program [`pointers.c`](examples/properties/pointers.c)
```
main() {
  int *ptr;
  int array[10];
  *ptr = 3;
  *(array + 10) = 4;
}
```
the command
```
cbmc --pointer-check pointers.c
```
finds that `ptr` may be null and that `array + 10` points beyond the end
of `array`:
```
line 4 dereference failure: pointer NULL in *ptr: FAILURE
line 4 dereference failure: pointer invalid in *ptr: FAILURE
line 4 dereference failure: deallocated dynamic object in *ptr: FAILURE
line 4 dereference failure: dead object in *ptr: FAILURE
line 4 dereference failure: pointer outside object bounds in *ptr: FAILURE
line 4 dereference failure: invalid integer address in *ptr: FAILURE
line 5 dereference failure: dead object in array[(signed long int)10]: SUCCESS
line 5 dereference failure:
  pointer outside object bounds in array[(signed long int)10]: FAILURE
```

These off-by-one errors caught with `--bounds-check` and `--pointer-check`
feel quite similar to each other, but they are different.
We always use these flags together.

## More pointer checks

* `--pointer-overflow-check` checks pointer arithmetic
  for arithmetic overflow and underflow.
* `--pointer-primitive-check` checks that all pointers are either valid or null
  (all pointers, not just dereferenced  pointers).

Arithmetic overflow in pointer arithmetic
is an issue because the CBMC heap model interprets a pointer as
an object id together with an offset into the object.  Pointer arithmetic
in C uses signed integers for offsets.  So it is theoretically possible for
the offset into the object to have a magnitude larger than the
maximium offset CBMC is using for objects on the heap.

Given the file [`pointer-overflow.c`](examples/properties/pointer-overflow.c)
```
#include <stdint.h>
main() {
  int array[10];
  int *x = array + SIZE_MAX;
}
```
the command
```
cbmc --pointer-check pointer-overflow.c
```
will succeed (because the bad pointer is never dereferenced) but
the command
```
cbmc --pointer-check --pointer-overflow-check pointer-overflow.c
```
will find the overflow
```
line 4 pointer arithmetic:
  dead object in array + (signed long int)18446744073709551615ul: SUCCESS
line 4 pointer arithmetic:
  pointer outside object bounds in array + (signed long int)18446744073709551615ul:
  FAILURE
```

EXAMPLE

## Malloc failure

* `--malloc-may-fail` allows any invocation of `malloc` to fail.
* `--malloc-fail-null` sets the `malloc` failure mode to returning
a null pointer.

Taken together, these flags cause CBMC
to use a model of `malloc` in which it is possible for any invocation
of `malloc` to fail and return a null pointer.  Given the
program [`malloc.c`](examples/properties/malloc.c)
```
#include <stdlib.h>
main () {
  int *x = (int *) malloc(sizeof(int));
  *x = 1;
}
```
the command
```
cbmc --pointer-check malloc.c
```
will succeed but the command
```
cbmc --pointer-check --malloc-may-fail --malloc-fail-null malloc.c
```
will fail because it is possible for `malloc` to fail and return a
null pointer causing `*x` to dereference a null pointer:
```
line 4 dereference failure: pointer NULL in *x: FAILURE
line 4 dereference failure: pointer invalid in *x: SUCCESS
line 4 dereference failure: deallocated dynamic object in *x: FAILURE
line 4 dereference failure: dead object in *x: FAILURE
line 4 dereference failure: pointer outside object bounds in *x: FAILURE
line 4 dereference failure: invalid integer address in *x: SUCCESS
```



## Integer overflow

* `--signed-overflow-check` and `--unsigned-overflow-check`
  check for integer overflow.

Given the file [integer.c](examples/properties/integer.c)
```
#include <limits.h>
main() {
  int x;
  int y;
  if (x + y <= INT_MAX)
    x = x + y;
  if (y <= INT_MAX - x)
    x = x + y;
}
```
the command
```
cbmc --signed-overflow-check integer.c
```
finds several possibilities for signed integer overflow
(an undefined behavior in the C standard):
```
line 5 arithmetic overflow on signed + in x + y: FAILURE
line 7 arithmetic overflow on signed - in 2147483647 - x: FAILURE
line 8 arithmetic overflow on signed + in x + y: FAILURE
```

## Floating point overflow

* `--float-overflow-check` checks for +/-Inf in floating-point arithmetic.
* `--nan-check` checks for NaN in floating-point arithmetic.

Given the file [`float.c`](examples/properties/float.c)
```
main() {
  float x = 100000000.0;
  for (int i = 0; i < 10; i++) x = x * x;
}
```
the command
```
cbmc float.c
```
will succeed but the command
```
cbmc --float-overflow-check float.c
```
will find the floating-point overflow:
```
line 3 arithmetic overflow on floating-point multiplication in x * x: FAILURE
```

Given the file [`nan.c`](examples/properties/nan.c)
```
main() {
  float x = 0.0 / 0.0;
}
```
the command
```
cbmc nan.c
```
will succeed but the command
```
cbmc --nan-check nan.c
```
will find the NaN:
```
line 2 NaN on / in 0.0 / 0.0: FAILURE
```

## Division by zero

* `--div-by-zero-check` checks for division by zero.

Given the file [`zero.c`](examples/properties/zero.c)
```
main() {
  int a;
  int b;
  int result = a / b;
}
```
the command
```
cbmc --div-by-zero-check zero.c
```
finds the potential for division by zero:
```
line 4 division by zero in a / b: FAILURE
```


## Type casting

* `--conversion-check` checks for values that can't be
  represented after type casts.

Given the file [`conversion.c`](examples/properties/conversion.c)
```
#include <stdint.h>
main() {
  uint8_t x;
  uint16_t y;
  x = y;
}
```
the command
```
cbmc conversion.c
```
will succeed but the command
```
cbmc --conversion-check conversion.c
```
finds the potential for truncation of an integer value from 16 bits to 8 bits:
```
line 5 arithmetic overflow on unsigned to unsigned type conversion in (uint8_t)y:
  FAILURE
```

## Bit shifting

* `--undefined-shift-check` enables checks for undefined shift behaviors like
  shifting too far or shifting a negative value

Given the program ['shift.c'](examples/properties/shift.c)
```
main() {
  int x = 1;
  int y = -1;
  int z;
  z = x << 64;
  z = x << -1;
  z = y << 64;
  z = y << -1;
}
```
the command
```
cbmc --undefined-shift-check shift.c
```
finds a number of undefined shift behaviors:
```
line 5 shift distance too large in x << 64: FAILURE
line 5 shift operand is negative in x << 64: SUCCESS
line 6 shift distance is negative in x << -1: FAILURE
line 6 shift operand is negative in x << -1: SUCCESS
line 7 shift distance too large in y << 64: FAILURE
line 7 shift operand is negative in y << 64: FAILURE
line 8 shift distance is negative in y << -1: FAILURE
line 8 shift operand is negative in y << -1: FAILURE
```

## Loop unwinding

* `--unwinding-assertions` enables checks that loops have been
  completely unwound.

Always run CBMC with the flag `--unwinding-assertions`.

Given the program [`loop.c`](examples/properties/loop.c)
```
#include <stdbool.h>
main() {
  int bound;
  for (int i=0; i<bound; i++)
    if (i > 9) assert(false);
}
```
the command
```
cbmc --unwind 10 loop.c
```
will report
```
line 5 assertion 0: SUCCESS
```
This is a surprise.  The assertion is false.  Why did CBMC not discover this?
The problem is that the assertion fails only on the 10th iteration of the
loop, and we gave CBMC the flag `--unwind 10` that restricts CBMC
to 9 iterations (not 10!).
Use the flag `--unwinding-assertions` to detect mistakes like this.
The command
```
cbmc --unwind 10 --unwinding-assertions loop.c
```
will report
```
line 4 unwinding assertion loop 0: FAILURE
line 5 assertion 0: SUCCESS
```

Always run CBMC with the flag `--unwinding-assertions`.
