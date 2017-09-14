#!/bin/bash

# File  : test.sh
# Author: Marco Bonelli
# Date  : 2017-09-14
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

function fail {
	printf "Testing failed! :(\n"
	rm -r $TMPDIR
	exit 1
}

function test_memory {
	cp gdb_test_mem_err.in $TMPDIR/gdb_in
	sed -i "s|_LOGFILE_|$TMPDIR/gdb_out|g" $TMPDIR/gdb_in

	gdb -q -batch ../build/simplefs -x $TMPDIR/gdb_in

	printf "Running memory error tests:  \n"

	printf "  [1/3] Test exit on failed malloc "
	grep -q "\$1 = 1" < $TMPDIR/gdb_out
	if [ $? -eq 0 ]; then
		printf " -> OK.\n"
	else
		printf " -> ERROR!\n"
		if [ $FORCE_TESTS -eq 1 ]; then
			FAILED=1
		else
			fail
		fi
	fi

	printf "  [2/3] Test exit on failed calloc "
	grep -q "\$2 = 1" < $TMPDIR/gdb_out
	if [ $? -eq 0 ]; then
		printf " -> OK.\n"
	else
		printf " -> ERROR!\n"
		if [ $FORCE_TESTS -eq 1 ]; then
			FAILED=1
		else
			printf "\n"
			fail
		fi
	fi

	printf "  [3/3] Test exit on failed realloc"
	grep -q "\$3 = 1" < $TMPDIR/gdb_out
	if [ $? -eq 0 ]; then
		printf " -> OK.\n"
	else
		printf " -> ERROR!\n"
		if [ $FORCE_TESTS -eq 1 ]; then
			FAILED=1
		else
			printf "\n"
			fail
		fi
	fi
}

function test_file {
	spacing=$((24 - ${#1}))
	fname=$(basename ${f%.in})

	printf "  [%2d/%2d] File \"%s\"" $2 $3 $1
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": working on it...\r"

	tstart=$(date +%s%6N)
	../build/simplefs < input/$fname.in > $TMPDIR/dummy_out
	tend=$(date +%s%6N)

	dt=$((tend - tstart))
	dt=$(bc <<< "scale=3; ${dt}/1000")

	printf "  [%2d/%2d] File \"%s\"" $2 $3 $1
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": %7.3fms" $dt

	cmp --quiet $TMPDIR/dummy_out output/$fname.out
	res=$?

	if [ $res -eq 0 ]; then
		printf " -> OK.\n"
	else
		printf " -> ERROR!\n"
		if [ $FORCE_TESTS -eq 1 ]; then
			FAILED=1
		else
			printf "\n"
			fail
		fi
	fi
}

function test_random {
    spacing=$((7 - ${#1} - ${#2}))
	res=0
	tot=0

	printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": working on it...\r"

	for i in $(seq 1 $2); do
		./random_fs.py -n $1 > $TMPDIR/dummy_in
		./random_fs.py -n $1 -o	> $TMPDIR/dummy_expected

		tstart=$(date +%s%6N)
		../build/simplefs < $TMPDIR/dummy_in > $TMPDIR/dummy_out
		tend=$(date +%s%6N)

		dt=$((tend - tstart))
		tot=$((tot + dt))
		avg=$(bc <<< "scale=3; ${tot}/${i}/1000")

		printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
		(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
		printf ": %9.3fms (avg) [%3d/%-3d]...\r" $avg $i $2

		cmp --quiet $TMPDIR/dummy_out $TMPDIR/dummy_expected
		res=$?

		if [ $res -ne 0 ]; then
			break
		fi
	done

	printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": %9.3fms (avg) [%3d/%-3d]" $avg $i $2

	if [ $res -eq 0 ]; then
		printf " -> OK.\n"
	else
		printf " -> ERROR!\n"
		if [ $FORCE_TESTS -eq 1 ]; then
			FAILED=1
		else
			printf "\n"
			fail
		fi
	fi
}

FAILED=0
FORCE_TESTS=0
TEST_MEMORY=0
TEST_FILES=0
TEST_RANDOM=0
OPTION_ERR=0

if [ -z "$*" ]; then
	TEST_FILES=1
else
	if [[ "$*" =~ ^force( +(force)?)*$ ]]; then
		FORCE_TESTS=1
		TEST_FILES=1
	else
		for option in $*; do
			if   [ "$option" = "force"  ]; then FORCE_TESTS=1;
			elif [ "$option" = "memory" ]; then TEST_MEMORY=1;
			elif [ "$option" = "files"  ]; then TEST_FILES=1;
			elif [ "$option" = "random" ]; then TEST_RANDOM=1;
			else
				if [ $OPTION_ERR -eq 0 ]; then
					printf "usage: %s [force] [all] [memory] [files] [random]\n" $0
					OPTION_ERR=1
				fi
				
				printf "error: unsupported option: \"%s\".\n" $option
			fi
		done
	fi
fi

if [ $OPTION_ERR -eq 1 ]; then
	exit 1
fi

export LC_NUMERIC="en_US.UTF-8"
TMPDIR=$(mktemp -d)

if [ $TEST_MEMORY -eq 1 ]; then
	printf "Running memory error tests...\r"

	test_memory

	printf "\n"
fi

if [ $TEST_FILES -eq 1 ]; then
	printf "Running all test files:\n"

	i=0
	n=$(ls input -1 | wc -l)

	for f in input/*.in; do
		((i++))
		test_file $f $i $n
	done

	printf "\n"
fi

if [ $TEST_RANDOM -eq 1 ]; then
	printf "Running randomly generated tests:\n"

	test_random 10 100 1 5
	test_random 100 10 2 5
	test_random 1000 10 3 5
	test_random 10000 5 4 5
	test_random 100000 1 5 5

	printf "\n"
fi

rm -r $TMPDIR

if [ $FAILED -eq 1 ]; then
	fail
else
	printf "Testing succeded! :)\n"
fi
