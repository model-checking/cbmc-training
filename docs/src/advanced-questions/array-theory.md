# Does CBMC have any support for using array theory?
[Article Date: Aug 10, 2022]

CBMC provides support for reasoning about arrays using array theory. (Note that we are still working with a SAT back-end here, and this does not use an SMT solver)
A fixed length array uses CBMC's typical bit-vector representation of an array, but a variable length array uses CBMC's array theory.
To activate CBMC's array-theory for fixed length arrays, use the following workaround.

```
unsigned long size;  // non deterministically pick the size
ArrayElement *a = malloc(size * sizeof(ArrayElement));
__CPROVER_assume(size == MAX_SIZE);  // fix the size to a constant after the allocation
```
[See https://github.com/diffblue/cbmc/issues/7012 to read more about the workaround and track the fix]

## Performance impact of using CBMC's array theory feature.

CBMC’s array theory generates a number of constraints that is quadratic in the number of accesses to an array, and could be slower in cases with large number of accesses (worse for write accesses). However, unlike the bit-vector encoding, the number of constraints is not dependent on the size of the array itself.

Hence, array theory is better when you have a small number of accesses with non-constant index into a very large array. An example of this is when you want to reason about a single insertion of an element at a non-constant index in a large array.

