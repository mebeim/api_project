#!/bin/bash

# File  : test.sh
# Author: Marco Bonelli
# Date  : 2017-07-28
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
	printf " -> ERROR!\n\nTesting failed! :(\n"
	rm -r $TMPDIR
	exit 1
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
		fail
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
		fail
	fi
}

if [ "$1" == "exit" ] || [ "$1" == "files" ] || [ "$1" == "random" ]; then
	TESTS=$1
else
	if [ "$1" == "all" ] || [ -z "$1"]; then
		TESTS="all"
	else
		printf "Usage: %s [all|exit|files|random]\n" $0
		printf "Error: unsuppported option: \"%s\".\n" $1
		exit 1
	fi
fi

export LC_NUMERIC="en_US.UTF-8"
TMPDIR=$(mktemp -d)

if [ "$TESTS" == "all" ] || [ "$TESTS" == "exit" ]; then
	printf "Running abnormal exit tests...\r"

	cp gdb_test_exit.in $TMPDIR/gdb_in
	sed -i "s|_LOGFILE_|$TMPDIR/gdb_out|g" $TMPDIR/gdb_in

	gdb -q -batch ../build/simplefs -x $TMPDIR/gdb_in

	printf "Running abnormal exit tests:  \n"

	printf "  [1/3] Test exit on failed malloc "
	grep -q "\$1 = 1" < $TMPDIR/gdb_out
	if [ $? -eq 0 ]; then
		printf " -> OK!\n"
	else
		fail
	fi

	printf "  [2/3] Test exit on failed calloc "
	grep -q "\$2 = 1" < $TMPDIR/gdb_out
	if [ $? -eq 0 ]; then
		printf " -> OK!\n"
	else
		fail
	fi

	printf "  [3/3] Test exit on failed realloc"
	grep -q "\$3 = 1" < $TMPDIR/gdb_out
	if [ $? -eq 0 ]; then
		printf " -> OK!\n"
	else
		fail
	fi

	printf "\n"
fi

if [ "$TESTS" == "all" ] || [ "$TESTS" == "files" ]; then
	printf "Running all test files:\n"

	i=0
	n=$(ls input -1 | wc -l)

	for f in input/*.in; do
		((i++))
		test_file $f $i $n
	done

	printf "\n"
fi

if [ "$TESTS" == "all" ] || [ "$TESTS" == "random" ]; then
	printf "Running random tests:\n"

	test_random 10 100 1 5
	test_random 100 10 2 5
	test_random 1000 10 3 5
	test_random 10000 5 4 5
	test_random 100000 1 5 5

	printf "\n"
fi

rm -r $TMPDIR

printf "Testing succeded! :)\n"
