Makefile naming conventions
===========================

`file.cpp`: Some file that contains source code of implementations.
If this file has a `main` function,
ideally it should be the only function of the file.
There shall not be any `file.h` headers acompanying
`file.cpp` if it contains a `main` function.

`file.h`: Some file that contains mainly declarations.
There shall be an acompanying `file.cpp` file
that implements everything declared in `file.h`,
and it implements *only* these functions
(it may, however, implement "private" functions).

`file.hpp`: Some file that contains mainly template code.
The code shall be fully implemented in `file.hpp`;
there shall not be any acompanying `cpp` file.

`file.dep.mk`: Makefile dependency file.
Generated and updated automatically.
These are only generated for cpp files.

`file.o`: Intermediate object file, created from a .cpp.
Generated and updated automatically.

`file`: A program.
Generated and updated automatically.
The makefile generates it if `file.cpp` contains a `main` function.


The "shall"s of this section are not mere pedantry;
the makefile assumes these rules to be true
in order to function properly.


Other file types
----------------

`file.dataset`: A dataset file.
See [`datasets/format.md`](datasets/format.md) for more information.
