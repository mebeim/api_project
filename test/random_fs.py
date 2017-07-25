#!/usr/bin/python3

# File  : random_fs.py
# Author: Marco Bonelli
# Date  : 2017-07-25
#
# Copyright (c) 2017 Marco Bonelli.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import sys
import argparse
import random
import string

def positive(value):
    ivalue = int(value)
    if ivalue <= 0:
         raise argparse.ArgumentTypeError("%s is an invalid positive integer" % value)
    return ivalue

def check_args(args):
    if args.o:
        return

    max_n = sum(args.w ** i for i in range(1, args.d + 1))
    if args.n > max_n:
        ap.error("requested number of files ({}) exceeding width and depth limits (max {} with -w {} -d {})".format(args.n, max_n, args.w, args.d))

    l = len(fname_chars_pool)
    max_n = sum(l ** i for i in range (1, args.f + 1))
    if args.w > max_n and args.n > max_n:
        sys.stderr.write("warning: specified width and number of files are greater than maximum different file names which can be generated; this may result in an incorrent list of commands (or even no list at all)\n")

def random_fname(length):
	return "".join(random.choice(fname_chars_pool) for i in range(length))

def write_commands(node, path):
    for key, item in node.items():
        if len(item):
            new_path = path + key
            sys.stdout.write("create_dir {}\n".format(new_path))
            write_commands(item, new_path)
        else:
            sys.stdout.write("create     {}\n".format(path + key))

fname_chars_pool = string.ascii_lowercase + string.ascii_uppercase + string.digits

ap = argparse.ArgumentParser(
    description     = '''Generate a random list of create/create_dir commands with random
                         alphanumeric file names. The generated output can be used as test
                         input for SimpleFS, which should result in exactly N "ok" lines
                         as output.''',
    formatter_class = argparse.ArgumentDefaultsHelpFormatter
)

ap.add_argument("-o", action = "store_true", help = "generate expected result from SimpleFS instead of random input; this will output N \"ok\" lines and thus every option except -n will be ignored")
ap.add_argument("-n", type = positive, default = 100000, help = "number of files to be created")
ap.add_argument("-f", type = positive, default = 255, help = "maximum file name length")
ap.add_argument("-e", action = "store_true", help = "use F as the exact length, not maximum")
ap.add_argument("-w", type = positive, default = 1024, help = "maximum number of children per directory")
ap.add_argument("-d", type = positive, default = 255, help = "maximum filesystem depth")

args = ap.parse_args()
check_args(args)

if args.o:
	for i in range(args.n):
		sys.stdout.write("ok\n")
else:
    fs = dict()
    n = args.n

    while n > 0:
        cur_dir = fs
        cur_depth = 0
        depth = random.randint(1, args.d)
    
        while cur_depth < depth and n > 0:
            if len(cur_dir) < args.w:
                next_fname = "/" + random_fname(args.f if args.e else random.randint(1, args.f))
                if next_fname not in cur_dir.keys():
                    cur_dir[next_fname] = dict()
                    n -= 1
            else:
                next_fname = random.choice(tuple(cur_dir.keys()))

            cur_dir = cur_dir[next_fname]
            cur_depth += 1

    write_commands(fs, "")
    sys.stdout.write("exit\n")
