SimpleFS - A simple file system
===============================

[![Build Status](https://travis-ci.org/mebeim/api_project.svg?branch=developing)](https://travis-ci.org/mebeim/api_project) [![Coverage](https://codecov.io/gh/mebeim/api_project/branch/developing/graph/badge.svg)](https://codecov.io/gh/mebeim/api_project)

This repo contains the final project of my Algorithms and principles of computer science (italian Algoritmi e Principi dell'Informatica, aka A.P.I.) course. The goal of the project is to implement a simple virtual filesystem API using C99 and the standard `libc`.

Source
------

The entire project has been split into different header and source files for clarity, but a complete source file ([`src/full_project.c`][1]), which is the one I had to submit for evaluation, containing all that's needed, is included. The license regulating use and redistribution of the source code is provided in the [`/LICENSE`][2] file.

Documentation
-------------

Check [`/doc/About.md`][3] for the detailed project assignment and description. The [`/doc`][4] folder also contains the original project assignment (in Italian) and a brief description of the data structure I used.

Source and header files contain explicative comments for every defined function, and more comments explaining each algorithm almost step by step can be found in the complete source file.

Building
--------

Required `cmake >= 2.8` (suggested `>= 2.8.4` on Cygwin)..

    ~$ git clone https://github.com/mebeim/api_project.git
    ~$ cd api_project
    ~/api_project$ mkdir build
    ~/api_project$ cd build
    ~/api_project/build$ cmake ..
    ~/api_project/build$ make

Testing
-------

After building, you can either test using `make` and specifying the `simplefs_test` target:

    ~/api_project/build$ make simplefs_test

Or directly calling the test script, which also accepts the options:

 - `files` to only run the test files in the `/test/input` folder and compare them with the expected outputs in `/test/output`;
 - `random` to only run randomly generated (`create`/`create_dir` only) test files (see [`/test/random_fs.py`][5] for more information);
 - `all`, which is the default if no option is specified, to run both files and random tests.

    ~/api_project/test$ ./test.sh all
    ~/api_project/test$ ./test.sh files
    ~/api_project/test$ ./test.sh random

-----------------------------------------------------------------------------

*Copyright &copy; 2017 Marco Bonelli. Licensed under the Apache License 2.0.*

 [1]: https://github.com/mebeim/api_project/blob/master/src/full_project.c
 [2]: https://github.com/mebeim/api_project/blob/master/LICENSE
 [3]: https://github.com/mebeim/api_project/blob/master/doc/About.md
 [4]: https://github.com/mebeim/api_project/tree/master/doc
 [5]: https://github.com/mebeim/api_project/blob/master/test/random_fs.py
