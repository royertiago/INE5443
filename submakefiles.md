Makefile structure
==================

In the light of
[Recursive Make Considered Harmful](http://aegis.sourceforge.net/auug97.pdf),
the makefiles of this project are not recursive.
Instead, the top make file (the file ./makefile)
includes other makefiles (for instance test/makefile.mk).
Some conventions ease this integration
while mantaining different build rules for each makefile.

The [file naming conventions](naming_conventions.md) shall be respected.

There are two variables,
MAIN and NOMAIN,
that can be used to chose what files will receive the default treatment
from the top makefile.
These variables can be amended (but not overwritten) by included makefiles.

MAIN contains every .cpp file that can be compiled to an executable;
that is, the files that contains the `main` function.
(Almost; see test/makefile.mk).

NOMAIN contains every .cpp file that implements some .h file.
Note that MAIN and NOMAIN shall be mutually excludent.

The top makefile will manage the automatic dependency generation.

The targets `clean` and `mostlyclean` can be extended
by providing extra dependencies.
The depended targets can, then, execute the cleaning code.
In fact, since the top makefile does not clean object files
in subdirectories that contains makefiles,
at lest the `moslyclean` target must be extended to do this.

To avoid name clashes, any extra target or variable
must be prefixed by the name of the makefile's directory.
For instance, test/makefile defines the variable TESTSRC
and the target test-clean.
