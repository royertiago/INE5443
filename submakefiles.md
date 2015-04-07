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

The top makefile will, by default,
create build rules for all object files and programs in the directory.
The presence of a submakefile supresses this rule generation.
The submakefile may use the top makefile's hooks
to have that functionality back
and to interact with the rest of the repository.

There are four variables,
in the main makefile,
that may be appended to to request such services.

-   `prog` lists all the programs of this repository.
    This variable will hold a list of targets.
    The object files in these targets' dependencies
    will be linked together to form the specified program.

-   `src` lists all the `.cpp` files that should be built
    into corresponding `.o` files.

-   `dep` lists all the `.dep.mk` files that will be automatically generated
    from the corresponding source files.

-   `obj` lists all the object files that provide symbol definitions
    that might be useful for other targets.
    (If the submakefile builds some shared object file,
    this variable is, in fact, mandatory.)
