INE5443 - Reconhecimento de Padrões
===================================
(_Pattern Recognition_)

INE5443 is an undergraduate course lectured by Aldo Wangenheim.
This repository contains the assignments of this course.


Organization
------------

This repository is a set of command-line tools
for pattern recognition.
It contains programs like classifiers
and dataset generators.

The programs are written in C++ and bash scripts.
The makefile assumes certaint filename conventions to work properly;
the details can be found in [`naming_conventions.md`](naming_conventions.md)
and [`submakefiles.md`](submakefiles.md).

For information about the dataset structure,
read [`datasets/format.md`](datasets/format.md).


Known bugs
==========

There is a bug in the makefile, in the module of tests.
There is a single hardcoded link between the test executable
and the object files that contain the auto-registering tests;
this link is made on-the-fly by test/makefile.mk,
based on currently existing object files in that directory.
This suffices to catch modification and inclusion of new test cases,
since the object file itself will be rebuilt before linking everything.

However, it does not detect file removals.

If one of these self-registering test object files get removed,
test/makefile.mk simply will not list it as a dependency of test/test.
If the removal was the only change, every prerequisite
will appear to make as up to date,
so the program test/test will not be recreated.

Workaround: execute `rm test/test` when deleting a test case.
