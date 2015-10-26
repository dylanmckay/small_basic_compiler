# Microsoft SmallBasic compiler

Uses LLVM to generate x86 machine code for a SmallBasic program.

A very small subset of the standard library is supported (see `stdlib/src`).

The project hasn't been touched in three years, but I thought it was worth putting on GitHub regardless.

There is a small test at `compiler/test.sb` which works. There are several tests in the `tests` directory that are taken from the SmallBasic [website](http://smallbasic.com). These do not run due to the fact that half of the standard library is missing. They were used as tests for the parser.
