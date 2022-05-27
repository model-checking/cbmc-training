# Coverage checking

CBMC can also do coverage checking.  You can ask CBMC to do property
checking with
```bash
cbmc program.goto --check1 --check2 --check3 --unwinding-assertions
```
and do coverage checking with
```bash
cbmc program.goto --check1 --check2 --check3 --cover location
```
The first command causes CBMC to do property checking,
and the second command causes CBMC to do coverage checking.
CBMC computes line coverage and computes the lines of code that CBMC was
able to exercise while doing the property checking.
Unexpectedly low code coverage is usually an indication that your proof
harness is not modeling enough program state or is otherwise
over-constraining the set of values the state can take on,
something you should probably investigate.
(Notice that the second command adds `--cover location`
and omits `--unwinding-assertions`; the two flags cannot be used together.)

[CBMC viewer](https://github.com/model-checking/cbmc-viewer) is a tool
that summarizes the findings of CBMC in the form of a report that can
be opened in any web browser.  One thing that viewer does is render
the coverage data in two forms.  First, it gives coverage data for the
individual functions and for the code as a whole.  Second,
in the style of [lcov](https://github.com/linux-test-project/lcov),
it annotates lines of source code with the colors
green and red to indicate the lines hit and missed by
CBMC.  The two representations of coverage work together to make it easier
to understand why coverage is lower than expected.
See the
[release page](https://github.com/model-checking/cbmc-viewer/releases/latest)
and the
[documentation](https://model-checking.github.io/cbmc-viewer/)
for more information.

The coverage data itself, without the aid of CBMC viewer, is a little
overwhelming for most users.  CBMC computes the set of basic blocks in the
goto program.  A basic block is a block of straight-line code ending with a
goto statement (going to the next basic block).  CBMC
annotates each basic block with a test that amounts to saying "CBMC entered
this basic block at least once during property checking".  CBMC uses the
SAT solver to determine if there is any input and any path through the code
that could reach this basic block.  The output is the list of basic
blocks, and for each basic block whether CBMC entered the basic block
and the lines of source code contributing steps to the basic
block.

Coverage checking can take longer than property checking.  Depending on
what properties you are checking for, there can be many more basic
blocks to check than there are properties to check.  As a result, the
constraint problem for coverage checking can be both bigger and harder
to solve than for property checking.
