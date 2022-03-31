# CBMC starter kit overview

The [CBMC starter kit](https://github.com/awslabs/aws-templates-for-cbmc-proofs)
makes it easy to add CBMC verification to a software project.

In this tutorial, we want to verify the memory safety of a memory allocator that
comes with the [FreeRTOS Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel).
The kernel comes with five allocators, and we look at the simplest one.
It allocates blocks from a region of memory set aside for the heap.
It maintains a linked list of free blocks and allocates a block from
the first block in the free list that is big enough to satisfy the request.
When the block is freed, it is added back to the free list and merged with
adjacent free blocks already in the free list.
The function we want to prove memory safe is the allocator
[`pvPortMalloc`](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/portable/MemMang/heap_5.c#L155)
in the source file
[portable/MemMang/heap_5.c](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/portable/MemMang/heap_5.c).

Using the starter kit consists of five steps
* [Clone the source repository](#clone-the-source-repository)
* [Add the starter kit](#add-the-starter-kit)
* [Configure the starter kit](#configure-the-starter-kit)
* [Configure the proof](#configure-the-proof)
* [Write the proof](#write-the-proof)
* [Run CBMC](#run-cbmc)

For each step, we give the shell commands required to implement
the step, and we examine what changes the commands have made to the repository.
A reader wanting
to push as rapidly as possible to the verification can simply cut-and-paste
the commands into a shell and begin the verification.  A reader wanting to
understand how the starter kit works can pause to read the explanations.

## Clone the source repository

### Commands

The first step is to clone the FreeRTOS Kernel repository and
its submodules, and to create a `cbmc` subdirectory to hold
everything related to CBMC verification:

```
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git kernel
cd kernel
git submodule update --init --checkout --recursive
mkdir cbmc
cd cbmc
```

### Explanation

The name of the directory `cbmc` and its location within the
repository is unimportant.  Just choose a reasonable place
within the repository.  For example, you could put `cbmc`
under a `test` or `verification` directory that contains your unit
tests and other verification work.

## Add the starter kit

### Commands

The next step is to add the starter kit as a submodule.

```
git submodule add https://github.com/model-checking/cbmc-starter-kit starter-kit
```

### Explanation

This just adds the starter kit as a submodule.

## Configure the starter kit

### Commands

The next step is to configure CBMC verification for this project and
copy the contents of the starter kit into place.
Let's run the setup script:
```
./starter-kit/scripts/setup.py
```
```
What is the project name? Kernel
```

### Explanation

Looking at the `cbmc` directory, we see that a few subdirectories have
been added to `cbmc`.
```
ls
```
```
include         proofs          sources         starter-kit     stubs
```
In addition to litani and the starter-kit submodules, we see
* `include` for header files written for verification.
* `sources` for common source files written for verification.
* `stubs` for any stubs of project code written for verification.
  Examples of useful stubs for a communication protocol might be models of
  the `send` and `receive` methods that interact with the physical network.
* `proofs` for the verification itself.  In the end, this directory will
  contain one subdirectory for every function or entry point we want to verify.
  This `proofs` directory is usually just a flat collection of subdirectories,
  but these subdirectories can be nested if it makes sense to group
  together the verification of similar functions.

The `proofs` directory itself is worth further examination.
```
ls proofs
```
```
Makefile-project-defines        Makefile.common
Makefile-project-targets        README.md
Makefile-project-testing        run-cbmc-proofs.py
Makefile-template-defines
```
The important files are
* `Makefile.common` implements best practices for verification using CBMC.
  It describes a method of building the goto binary for
  CBMC that is likely to yield good CBMC performance, and it describes
  a set of checks we recommend performing with CBMC.  It invokes CBMC
  several times to do property checking and coverage checking (currently
  just line coverage and not branch coverage), and it
  invokes a tool that builds a dashboard of the results in a form
  that is easy to debug.  It works by calling
  `litani` to perform all of these build and verification steps with
  maximal concurrency.
* `run-cbmc-proofs.py` is a script that runs with maximal concurrency
  all of the CBMC verification under the `proofs` directory, and builds
  a dashboard of the results.
  In another tutorial, we will introduce our
  implementation of continuous integration that runs all of the CBMC verification
  against each pull request to check that proposed code changes
  preserve the CBMC results.  This is the script invoked by continuous integration
  to run the verification.

The remaining Makefiles are just hooks for describing project-specific
modifications or definitions.  For example, within `Makefile-project-defines`
you can define the `INCLUDES` variable to set the search path for the header
files needed to build the project functions being verified.

## Configure the proof

### Commands

The next step is to configure CBMC verification of  the memory allocator
[`pvPortMalloc`](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/portable/MemMang/heap_5.c#L155)
in the source file
[portable/MemMang/heap_5.c](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/portable/MemMang/heap_5.c).  Let's run the setup script:
```
cd proofs
../starter-kit/scripts/setup-proof.py
```
```
What is the function name? pvPortMalloc
These source files define a function 'pvPortMalloc':
   0 ../../portable/ARMv8M/secure/heap/secure_heap.c
   1 ../../portable/GCC/ARM_CM23/secure/secure_heap.c
   2 ../../portable/GCC/ARM_CM33/secure/secure_heap.c
   3 ../../portable/IAR/ARM_CM23/secure/secure_heap.c
   4 ../../portable/IAR/ARM_CM33/secure/secure_heap.c
   5 ../../portable/MemMang/heap_1.c
   6 ../../portable/MemMang/heap_2.c
   7 ../../portable/MemMang/heap_3.c
   8 ../../portable/MemMang/heap_4.c
   9 ../../portable/MemMang/heap_5.c
  10 ../../portable/WizC/PIC18/port.c
  11 The source file is not listed here
Select a source file (the options are 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11): 9
```

### Explanation

The `proofs` directory now contains a subdirectory `pvPortMalloc`
for verification of the memory allocator `pvPortMalloc`.
```
ls pvPortMalloc
```
```
Makefile                cbmc-proof.txt
README.md               pvPortMalloc_harness.c
```
The important files are
* `pvPortMalloc_harness.c` is the start of a proof harness for `pvPortMalloc`.
* `Makefile` drives building and linking `pvPortMalloc` and its proof harness
  and running CBMC on the result.

## Write the proof

Let's change to the `pvPortMalloc` subdirectory:
```
cd pvPortMalloc
```

### Building source code

Every software project has its own build procedure.  It is quite likely
that if you can build the project for execution with `make CC=gcc` then
you can build the project for model checking by CBMC with `make CC=goto-cc`.
The compiler
`goto-cc` is a drop-in replacement for `gcc` that compiles
source code into a *goto program* used by CBMC.  In theory, you can link
your proof harness against this goto program and run CBMC on the result.
In practice,
the result is huge and the performance of CBMC is disappointing.
Our approach is to build only the source code that is required to run
the function under test.

In our case, all we need is the single file
[kernel/portable/MemMang/heap_5.c](https://github.com/FreeRTOS/FreeRTOS-Kernel/blob/main/portable/MemMang/heap_5.c).

Building FreeRTOS requires a configuration file, so let's add to the current
directory a slightly simplified [FreeRTOSconfig.h](FreeRTOSconfig.h) that we have
taken from another repository that demonstrates FreeRTOS.
Building FreeRTOS also requires an implementation of a portable interface, so
let's use an implementation in `kernel/portable/ThirdParty/GCC/Posix` that comes
with the kernel repository.

Now we can build heap_5.c with
```
gcc -I. -I../../include -I../../portable/ThirdParty/GCC/Posix \
  ../../portable/MemMang/heap_5.c
```

### The proof harness

Let's look at the initial outline of the proof harness `pvPortMalloc_harness.c`:
```c
/**
 * @file pvPortMalloc_harness.c
 * @brief Implements the proof harness for pvPortMalloc function.
 */

/*
 * Insert project header files that
 *   - include the declaration of the function
 *   - include the types needed to declare function arguments
 */

void harness()
{

  /* Insert argument declarations */

  pvPortMalloc( /* Insert arguments */ );
}
```

The first thing we need to do is add the prototype for `pvPortMalloc`:
```
#include <stdlib.h>
void * pvPortMalloc( size_t xWantedSize );
```

The second thing we need to do is allocate an unconstrained value
for `xWantedSize` and pass it to `pvPortMalloc`:
```
  size_t xWantedSize;
  pvPortMalloc( xWantedSize );
```
The final proof harness `pvPortMalloc_harness.c` is:
```c
/**
 * @file pvPortMalloc_harness.c
 * @brief Implements the proof harness for pvPortMalloc function.
 */

#include <stdlib.h>
void * pvPortMalloc( size_t xWantedSize );

void harness()
{
  size_t xWantedSize;
  pvPortMalloc( xWantedSize );
}
```
### The makefile

Let's look at the initial outline of `Makefile`:
```
HARNESS_ENTRY = harness
HARNESS_FILE = pvPortMalloc_harness

# This should be a unique identifier for this proof, and will appear on the
# Litani dashboard. It can be human-readable and contain spaces if you wish.
PROOF_UID = pvPortMalloc

DEFINES +=
INCLUDES +=

REMOVE_FUNCTION_BODY +=
UNWINDSET +=

PROOF_SOURCES += $(PROOFDIR)/$(HARNESS_FILE).c
PROJECT_SOURCES += $(SRCDIR)/portable/MemMang/heap_5.c

# If this proof is found to consume huge amounts of RAM, you can set the
# EXPENSIVE variable. With new enough versions of the proof tools, this will
# restrict the number of EXPENSIVE CBMC jobs running at once. See the
# documentation in Makefile.common under the "Job Pools" heading for details.
# EXPENSIVE = true

include ../Makefile.common
```

All we need to do here is to define the search path for the needed header
files.  Since the variable `$(SRCDIR)` points to the root of the repository
(the source root), and the variabld `$(PROOFDIR)` points to the current
proof directory, all we need to do is add the lines
```
INCLUDES += -I$(PROOFDIR)
INCLUDES += -I$(SRCDIR)/include
INCLUDES += -I$(SRCDIR)/portable/ThirdParty/GCC/Posix
```

## Run CBMC

Now we can run CBMC on `pvPortMalloc` with
```
make
```
This takes about 30 seconds on MacOS.
It builds the proof harness, builds the relevant source code, links them
together, runs CBMC several times to do property checking and coverage checking,
and runs another tool to build a summary of the results that can be opened
in a web browser.  On MacOS, open the report with
```
open report/html/index.html
```
and study the results.

With this, in just minutes, you have used the starter kit to get started
writing CBMC proofs for the FreeRTOS kernel.  The proof is not done.  If you
look at the report you have opened in your web browser, you can see that
the current proof harness fails to exercise a large fraction of the function
`pvPortMalloc`.  For example, the proof harness currently fails to allocate
the region of memory from which `pvPortMalloc` should allocate blocks
of memory.  But this is where the fun begins...
