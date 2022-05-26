# CBMC quick start

[CBMC](https://github.com/diffblue/cbmc) is a model checker for
C. This means that CBMC will explore all possible paths through your code
on all possible inputs, and will check that all assertions in your code are
true.
CBMC can also check for the possibility of
memory safety errors (like buffer overflow) and for instances of
undefined behavior (like signed integer overflow).
CBMC is a bounded model checker, however, and using CBMC may require
restricting inputs to inputs of some bounded size.
The result is assurance that your code behaves as expected for all
such inputs.

CBMC has been used on
over a dozen [industrial software verification projects](projects.md)
as a routine part of software development and continuous integration.
This experience has resulted in some "best practices" for how to
build software for CBMC and how to run CBMC in development.
The CBMC starter kit is an implementation of these best practices,
and is intended to make it easy to deploy CBMC to a software project
as part of software development and continuous integration.

This document is a guide to using CBMC and to deploying CBMC to
an industrial software verification project.

## Getting started

For a quick start on writing simple proofs, read
  * [CBMC installation](installation.md)
  * [CBMC as unit testing](cbmc/overview/unit-testing.md)
  * [CBMC as debugging](cbmc/overview/introduction.md)
  * [CBMC proofs](cbmc/overview)

For a quick start on deploying CBMC to a software project
[using the starter kit](starter-kit/overview)

  * Read [configure the repository](starter-kit/overview#configure-the-repository)
    to set up a software repository for CBMC proof
  * Read [configure the proof](starter-kit/overview#configure-the-proof)
    to add a new CBMC proof to an existing set of CBMC proofs
  * Read [run all proofs](starter-kit/overview#run-all-the-proofs)
    to run a set of existing CBMC proofs and examine the results.

For advice on planning and managing a CBMC proof project,
read [CBMC project management](management).

To learn more, see our list of [CBMC resources](resources.md) and
[CBMC projects](projects.md), and see
  * CBMC starter kit [reference manual](https://model-checking.github.io/cbmc-starter-kit/reference-manual) and [documentation](https://model-checking.github.io/cbmc-starter-kit)
  * CBMC viewer [reference manual](https://model-checking.github.io/cbmc-viewer/reference-manual) and [documentation](https://model-checking.github.io/cbmc-viewer)
  * CBMC [developer guide](http://cprover.diffblue.com/)
  * Litani [reference manual](https://awslabs.github.io/aws-build-accumulator)

## Helping others

This training material is a work in progress.  If you have suggestions,
corrections, or questions, please contact us by submitting a
[GitHub issue](https://github.com/model-checking/cbmc-training/issues).
If you have some training of your own that you would like to contribute,
please submit your contributions as a
[GitHub pull request](https://github.com/model-checking/cbmc-training/pulls).
