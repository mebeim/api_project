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

function test_file {
	spacing=$((24 - ${#1}))
	fname=$(basename ${f%.in})

	if [ $PRETTYPRINT -eq 1 ]; then
		printf "  [%2d/%2d] File \"%s\"" $2 $3 $1
		(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
		printf ": working on it...\r"
	fi

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
		printf " -> ERROR!\n\nTesting failed! :(\n"
		exit 1
	fi
}

function test_random {
    spacing=$((7 - ${#1} - ${#2}))
	res=0
	tot=0

	if [ $PRETTYPRINT -eq 1 ]; then
		printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
		(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
		printf ": working on it...\r"
	fi

	for i in $(seq 1 $2); do
		./random_fs.py -n $1 > $TMPDIR/dummy_in
		./random_fs.py -n $1 -o	> $TMPDIR/dummy_expected

		tstart=$(date +%s%6N)
		../build/simplefs < $TMPDIR/dummy_in > $TMPDIR/dummy_out
		tend=$(date +%s%6N)

		dt=$((tend - tstart))
		tot=$((tot + dt))
		avg=$(bc <<< "scale=3; ${tot}/${i}/1000")

		if [ $PRETTYPRINT -eq 1 ]; then
			printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
			(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
			printf ": %9.3fms (avg) [%3d/%-3d]...\r" $avg $i $2
		fi

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
		printf " -> ERROR!\n\nTesting failed! :(\n"
		exit 1
	fi
}

#[ "$CI" = "true" ] && [ "$TRAVIS" = "true" ]
#PRETTYPRINT=$?
PRETTYPRINT=1
TMPDIR=$(mktemp -d)

printf "Running all test files:\n"

i=0
n=$(ls input -1 | wc -l)

for f in input/*.in; do
	((i++))
	test_file $f $i $n
done

printf "\nRunning random tests:\n"

test_random 10 100 1 5
test_random 100 10 2 5
test_random 1000 10 3 5
test_random 10000 5 4 5
test_random 100000 1 5 5

rm -r $TMPDIR

printf "\nTesting succeded! :)\n"
