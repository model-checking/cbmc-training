# Getting started with CBMC

[CBMC](https://github.com/diffblue/cbmc) is a model checker for
C. This means that CBMC will explore all possible paths through your code
on all possible inputs, and will check that all assertions in your code are
true.
CBMC can also check for the possibility of
memory safety errors (like buffer overflow) and for instances of
undefined behavior (like signed integer overflow).
CBMC is a bounded model checker, however, which means that using CBMC may
require restricting this set of all possible inputs to inputs of some
bounded size.
CBMC has been used on
[over a dozen software projects](projects.md) as part of software
development and continuous integration.

This book is a collection of tutorials and reference guides for using
CBMC in software verification.

## Getting started

For a quick start on simple problems, read
  * [CBMC installation](installation.md)
  * [CBMC introduction](cbmc/overview/introduction.md)
  * [CBMC as unit testing](cbmc/overview/unit-testing.md)
  * [Using CBMC](cbmc/overview)

For a quick start on using the CBMC starter kit to add CBMC verification
to an existing software project, read
  * [Using CBMC on a project]()

For more information, see our list of [CBMC resources](resources.md).

## Helping others

This training material is a work in progress.  If you have suggestions,
corrections, or questions, contact us by submitting a
[GitHub issue](https://github.com/model-checking/cbmc-training/issues).
If you have some training of your own that you would like to contribute,
submit your contributions as a
[GitHub pull request](https://github.com/model-checking/cbmc-training/pulls).
