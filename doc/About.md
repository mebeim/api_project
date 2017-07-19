SimpleFS - A simple filesystem
==============================

### June 20, 2017

This file is a translated version of the [original assignment file][1] (which was provided in italian).

## 1. Summary

The objective of the Algorithms and principles of computer science project is to implement a simple hierarchic filesystem only stored in main memory. A hierarchic filesystem organizes its resources in a tree structure and uniquely identifies each resource with its path from the root. Resources contained in a hierarchic filesystem can be *files* or *directories*. Both of them have a name, represented as a string of characters. Files can only be inserted in the tree as leafs, while directories can appear both as leafs and intermediate nodes. The root of the tree is conventionally a directory, called "root directory" indeed. Only file nodes can contain data, represented as a sequence of bytes, while directories don't have associated data. Every node of the tree can contain metadata, but, for the purpose of this project, only directories do. The metadata of a directory are the names of its direct descendants.

The program implementing the filesystem will take a list of actions to perform from the standard input, and will print their result on the standard output. The program must be implemented in standard C99, with the only help of the standard library (`libc`) and basic runtime. The program is supposed to read one line from the list of actions, perform the corresponding operation on the internally maintained filesystem representation, and print out the result on standard output before continuing to the next action (the execution of the actions is completely sequential).

## 2. Input format

The filesystem's paths are represented using the usual UNIX syntax: a path is therefore a sequence of names of resources which, from the root directory, reach the resource identified by the path itself. The names are separated by the path separator `/`. 

As an example, consider the following filesystem:

    [root directory]
           |
           +--[file0]
           |
           +--[dir1]
           |
           +--[dir2]
                |
                +--[file1]
                |
                +--[file2]
                |
                +--[file3]

The path identifying the resource `file0` is `/file0`, the one identifying `file3` is `/dir2/file3`.

The following resctriction apply:

 - Resource names are alphanumeric and at most 255 characters long.
 
 - The maximum tree height is 255.
 
 - The maximum number of childrens for a node is 1024.

The program receives one of the following commands for each line of the action list given as input, where `<path>` indicates a generic path, and `<name>` an alphanumeric string at most 255 characters long:

 - `create <path>`: Create an empty file (i.e. without any associated data) in the filesystem and print out "ok" if the file has been correctly created, or "no" if the creation didn't succeed (e.g. attempt to create a file in an inexistent directory, or exceeding the filesystem limits).
 
 - `create_dir <path>`: Create an empty directory in the filesystem and pring out "ok" if the directory has been correctly created, or "no" if the creation didn't succeed.
 
 - `read <path>`: Read the full content of a file, printing out "contenuto" followed by a space and the content of the file if the file exists, or "no" if the file doesn't exist.
 
 - `write <path> <content>`: Write (as a whole) the content of a file, which must already exist, overwriting any existing content, then print out "ok" followed by a space and the number of written characters if the operation succeeded, or "no" otherwise. The `<content>` parameter is a sequence of alphanumeric characters and spaces delimited by double quote characters; for example: `write /poems/jabberwocky "It was a brillig and the slithy toves"`.
 
 - `delete <path>`: Delete a resource, printing out the outcome ("ok"-"no"). A resource is only deletable if it hasn't got children.
 
 - `delete_r <path>`: Delete a resource and everyone of its descendants (if present), printing out the outcome ("ok"-"no").
 
 - `find <name>`: Find the position of any resource named `<name>` in the filesystem. Print out "ok" followed by a space and the path of the resource for any matching resource. The paths in the output must be printed one per line in lexicographic order. If no resource with the given name is found, print out "no".
 
 - `exit`: Terminate the execution of the program. Don't output anything.

## 3. Time complexity requirements

Let `l` be the length of a path, `d` the number of resources in the filesystem, `d_path` the number of resources which are descendants of the one specified by the path, and `f` the number of resources found in a research, the time complexities expected from the specified primitive functions in the preceeding sections are the following.

| Command      | Complexity                  |
|--------------|-----------------------------|
| `create`     | O(`l`)                      |
| `create_dir` | O(`l`)                      |
| `read`       | O(`l + \|<file content>\|`) |
| `write`      | O(`l + \|<file content>\|`) |
| `delete`     | O(`l`)                      |
| `delete_r`   | O(`d_path`)                 |
| `find`       | O(`d` + `f^2`)              |

 [1]: https://github.com/mebeim/api_project/blob/master/doc/Progetto%20v1.pdf