SimpleFS - A simple file system
===============================

[![license][license-img]][license-link]
[![travis][travis-img]][travis-link]
[![codecov][codecov-img]][codecov-link]

This repo contains the final project of my Algorithms and principles of computer science (italian Algoritmi e Principi dell'Informatica, aka A.P.I.) course. The goal of the project is to implement a simple virtual filesystem API using C99 and the standard `libc`.

Source
------

The entire project has been split into different header and source files for clarity, but a standalone complete source file full of comments ([`src/full_project.c`][1]), which is the one I had to submit for evaluation, is included.

Documentation
-------------

Check [`/doc/About.md`][2] for the detailed project assignment and description. The [`/doc`][3] folder also contains the original project assignment (in Italian) and a brief description of the data structure I used.

Source and header files contain explicative comments for every defined function, and more comments explaining each algorithm almost step by step can be found in the complete source file.

Building
--------

Required `cmake >= 2.8`.

    $ git clone https://github.com/mebeim/api_project.git
    $ cd api_project
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

Testing
-------

The test script can be run with any combination of the following options:

 - `memory` to run memory error tests (**requires `gdb`** to be installed);
 - `files` to run the test files (`/test/input` and check result with `/test/output`);
 - `random` to run randomly generated test files (see [`/test/random_fs.py`][4] for more info);
 - `all` to run all the tests.
 - `force` to continue running all tests instead of stopping at the first failure.

The default (if no options are specified) is `files`.

	$ ./test.sh Open the pod bay doors, HAL.
	usage: ./test.sh [force] [all] [memory] [files] [random]
	error: unsupported option: "Open".
	error: unsupported option: "the".
	error: unsupported option: "pod".
	error: unsupported option: "bay".
	error: unsupported option: "doors,".
	error: unsupported option: "HAL.".


**Note that testing generating random files will create very large temporary input files (`> 1GB`) during the execution of the script**.

-----------------------------------------------------------------------------

*Copyright &copy; 2017 Marco Bonelli. Licensed under the Apache License 2.0.*

 [travis-image]: https://travis-ci.org/mebeim/api_project.svg?branch=master

 [1]: https://github.com/mebeim/api_project/blob/master/src/full_project.c
 [2]: https://github.com/mebeim/api_project/blob/master/doc/About.md
 [3]: https://github.com/mebeim/api_project/tree/master/doc
 [4]: https://github.com/mebeim/api_project/blob/master/test/random_fs.py

 [license-img]:   https://img.shields.io/github/license/mebeim/api_project.svg
 [license-link]:  https://github.com/mebeim/api_project/blob/master/LICENSE
 [travis-img]:    https://img.shields.io/travis/mebeim/api_project/master.svg
 [travis-link]:   https://travis-ci.org/mebeim/api_project
 [codecov-img]:   https://img.shields.io/codecov/c/github/mebeim/api_project/master.svg
 [codecov-link]:  https://codecov.io/gh/mebeim/api_project/branch/master
