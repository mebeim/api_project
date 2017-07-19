SimpleFS - A simple file system
===============================

[![Build Status](https://travis-ci.org/mebeim/api_project.svg?branch=developing)](https://travis-ci.org/mebeim/api_project)

This repo contains the final project of my Algorithms and principles of computer science (italian Algoritmi e Principi dell'Informatica, aka A.P.I.) course. The goal of the project is to implement a simple virtual filesystem API using C99 and the standard `libc`.

Source
------

The entire project has been split into different header and source files for clarity, but a complete source file ([`src/full_project.c`][1]), which is the one I had to submit for evaluation, containing all that's needed, is included. The license regulating use and redistribution of the source code is provided in the [`/LICENSE`][2] file.

Documentation
-------------

The detailed project assignment and description can be found in the [`/doc`][3] folder, along with a simple description of the data structure used. The original project assignment file ([`/doc/Progetto v1.pdf`][4]) is in italian.

Source and header files contain explicative comments for every defined function, and more comments explaining each algorithm almost step by step can be found in the complete source file.

Building
--------

Required `cmake >= 2.8.8` and `gcc >= 5`.

	$ git clone https://github.com/mebeim/api_project.git
	$ cd api_project
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

-----------------------------------------------------------------------------

*Copyright &copy; 2017 Marco Bonelli. Licensed under the Apache License 2.0.*

 [1]: https://github.com/mebeim/api_project/blob/master/src/full_project.c
 [2]: https://github.com/mebeim/api_project/blob/master/LICENSE
 [3]: https://github.com/mebeim/api_project/tree/master/doc
 [4]: https://github.com/mebeim/api_project/blob/master/doc/Progetto%20v1.pdf
