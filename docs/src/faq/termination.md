# How do I debug why CBMC won't stop?

CBMC goes through several stages.  CBMC can theoretically get stuck in any
stage, but the most commonly slow stages are
* Doing the symbolic execution
* Generating the SAT formula from the symbolic execution
* Solving the SAT formula with the SAT solver

Almost always the problem is just one or two lines of code.  For example,
under certain circumstances something as simple as
```
  memcpy(dst, src, length);
```
can cause CBMC to become stuck generating the SAT formula that results from
considering all places `dst` and `src` might be pointing.

The first problem is to find the line of code.  Most programmers are
used to delta debugging in which the programmer performs a kind of
binary search on the code to find the problem.  Delete more and more
from the bottom of the function under test until CBMC no longer hangs.
The problem is in the last block of code (the last line) deleted that
caused CBMC to terminate.

The second problem is to fix the problem.  Usually the solution involves
some form of function abstraction.  It is a known problem, for example,
for the standard library functions `memcpy`, `memmove`, and 'memset'
to be a problem.  It is a common solution to abstract these functions
to havoc the entire destination object and not just the portion being
updated.  This works because functions under test commonly copy data to
the output and never look at the data actually copied.  A simple stub
for `memcpy` would be
```C
void memcpy(char *dst, char *src, size_t length) {
  __CPROVER_havoc(dst);
```
A better stub would check that `src` and `dst` are nonnull pointers to
nonoverlapping regions of memory.  For a good stub, see [memcpy](https://github.com/awslabs/aws-c-common/blob/main/verification/cbmc/stubs/memcpy_override_havoc.c) in the [AWS C Common](https://github.com/awslabs/aws-c-common) repository.

A final tip is that sometimes it is interesting to know which stage
of CBMC is taking so long.  The
output of CBMC can general indicate where the problem is.  Running CBMC with
```
  cbmc --flush
```
will flush to the terminal log messages saying that a given stage
is starting or finishing.  This usually gives some hint.  The most
difficult stage to debug is why CBMC is taking so long to generate
the SAT formula.  If the problem is the SAT solver is taking too long,
you may be able to figure out what is going on by running CBMC with
one flag at a time (eg, check `--bounds-check` and then check
`--unsigned-overflow-check`) until you can identify the check that is
taking so much time.
