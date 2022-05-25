# How does CBMC work?


The most complete descriptions of how CBMC works are

* [Behavioral Consistency of C and Verilog Programs Using Bounded Model
  Checking](http://www.kroening.com/papers/dac2003.pdf)
* [Behavioral Consistency of C and Verilog Programs](
  http://reports-archive.adm.cs.cmu.edu/anon/2003/CMU-CS-03-126.pdf)
* [CProver developer documentation](http://cprover.diffblue.com)

The first two documents are the original paper on CBMC and a follow-up
technical report giving a bit more detail.  They are excellent introductions,
but they are a bit old.  The third document is a collection documents for
developers.  They provide guidance on how to understand the source code,
but give an uneven coverage of the material.

CBMC uses a technique called
[symbolic execution](https://en.wikipedia.org/wiki/Symbolic_execution)
to turn a program into a set of symbolic constraints that describe
the behavior of the program (the paths through the program).

For example, consider the following code fragment
```
  int x;
  x = x+1;
```
CBMC sees the first statement and knows that `x` should be represented by
a vector of 64 binary variables describing the 64-bit binary value of `x`.
CBMC sees the second statement and generates a constraint that says,
"If `X0` and `X1` are binary vectors describing the value of `x`
before and after the assignment, then `X1` is `X0` plus 1."  CBMC uses
a Boolean description of a binary adder that adds the 64 bits of `X0` and
the 64 bits of `1`, and generates a constraint that the i-th bit of `X1` is
equal to the Boolean expression for the i-th bit of the adder's output
on inputs `X0` and `1`.

Proceeding in this way, statement by statement, CBMC constructs a collection
of constraints that describe the program behavior.  The constraints
describe as a collection of Boolean formulas how the
state of the program changes during an execution.

CBMC then formulates the following question for each property we
want to check about the program: "Does there exist an input and a path
through the program that violates the property?"  CBMC forumlates
this question as a constraint problem, and hands the constraint problem
to a constraint solver called a SAT solver.  The SAT solver looks for
an assignment of values to variables that satisfies the constraint
describing what a property violation would look like.

If the SAT solver can produce a satisfying assignment, then CBMC can take the
satisfying assignment and --- essentially reversing the process of generating
the constraints in the first place --- use the assignment to generate an
error trace describing the input (the initial state) and each step through
the program leading to a violation of the property.

If the SAT solver cannot produce a satisfying assignment, this amounts
to a proof that there is no input and no path through the program that
can violate the property.  Taking all these properties together,
if these properties describe memory safety, then this amounts to a proof
that the program is memory safe.

A proof of memory safety, of course, assuming all the assumptions the
proof harness is
making.  See our discussion of [proof assumptions](proof-assumptions.md) for more invormation.
