SimpleFS - A simple filesystem
==============================

This program emulates a fairly simple virtual filesystem which entirely resides in the RAM (i.e. no real file is created or modified, no access to disk is made) controlled by the user with a limited set of commands. A quick description of the data structure used follows.

Data structure
--------------

The filesystem itself consists of a large hash table which is used to store each file. In addition to this, the files are also organized in an N-ary tree structure to keep the commands' complexity within reasonable bounds.

### The files

A file is a `struct` containing:
  - its hash (i.e. index in the hash table);
  - the file name;
  - a boolean field indicating whether it is a directory or not;
  - a children count;
  - its content (in case it isn't a directory);
  - references to: its parent, its leftmost child (if any), its left sibling and
    its right sibling.

### The hash table

The hash table representing the filesystem is an array of pointers to files. A barely readapted C version of the MetroHash64 hash function, which takes a string as key and an ptional seed, is used for hashing. Closed hashing with linear probing is used to solve collisions. The initial size of the hash table is 1MiB, which should be large enough to never require an expansion (with consequent rehashing) in most cases, however both expansion and rehashing functions are implemented (and tested) for completeness sake.

To maintain every single file of the filesystem in the same hash table, the full path of a file would be a good key to use, but it is not practical to store such a long string for every file: it would consume too much memory. For this reason, each file only stores its name as a partial key, and the hash is calculated "piece by piece" using the parent file's hash as seed, going down the path from the root and looking at each folder's name until reaching the desired file.

### The N-ary tree

Since that each file has a references to its parent, its closest right sibling, and, in case of a directory, its leftmost child, each file is in fact also a node of an N-ary tree. Without a tree structure, and using only the hash table, it would be impossible to explore the filesystem (or even know where the children of a given folder are) in a reasonable amount of time.
