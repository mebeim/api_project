# api_project

[![Build Status](https://travis-ci.org/mebeim/api_project.svg?branch=master)](https://travis-ci.org/mebeim/api_project)

This repo contains the final project of my Algorithms and principles of computer science (italian Algoritmi e Principi dell'Informatica, aka A.P.I.) course.

## Source

The entire project has been split into different header and source files for clarity, but a complete source file ([`src/full_project.c`][1]), which is the one I had to submit for evaluation, containing all that's needed, is included. The license regulating use and redistribution of the source code is provided in the file [`/LICENSE`][2].

## Documentation

The project assignment and description can be found in the [`/doc`][3] folder, along with a simple description of the data structure used. The project assignment PDF which describes the program in detail is in italian.

Source and header files contain explicative comments for every defined function, and more comments explaining each algorithm almost step by step can be found in the complete source file.

## Building

	$ git clone https://github.com/mebeim/api_project.git
	$ cd api_project
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

 [1]: https://github.com/mebeim/api_project/blob/master/src/full_project.c
 [2]: https://github.com/mebeim/api_project/blob/master/LICENSE
 [3]: https://github.com/mebeim/api_project/tree/master/doc