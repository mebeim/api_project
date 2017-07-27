#!/bin/bash

# File  : test.sh
# Author: Marco Bonelli
# Date  : 2017-07-27
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

function goback {
	# Goes $1 lines back in the console output.
	tput cuu $1 && tput el
}

function test_random {
    spacing=$((7 - ${#1} - ${#2}))
	res=0
	tot=0

	printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": working on it...\n"

	for i in $(seq 1 $2); do
		./random_fs.py -n $1 > dummy_in
		./random_fs.py -n $1 -o	> dummy_expected

		tstart=$(date +%s%6N)
		../build/simplefs < dummy_in > dummy_out
		tend=$(date +%s%6N)

		dt=$((tend - tstart))
		tot=$((tot + dt))
		avg=$(bc <<< "scale=3; ${tot}/${i}/1000")

		goback 1
		printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
		(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
		printf ": %9.3fms (avg) [%3d/%-3d]...\n" $avg $i $2

		cmp --quiet dummy_out dummy_expected
		res=$?

		if [ $res -ne 0 ]; then
			break
		fi
	done

	rm dummy_*

	goback 1
	printf "  [%d/%d] %d random files (x%d)" $3 $4 $1 $2
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": %9.3fms (avg) [%3d/%-3d]" $avg $2 $2

	if [ $res -eq 0 ]; then
		printf " -> OK.\n"
	else
		printf " -> ERROR!\n\nTesting failed! :(\n"
		exit 1
	fi
}

function test_file {
	spacing=$((24 - ${#1}))
	fname=$(basename ${f%.in})

	printf "  [%2d/%2d] File \"%s\"" $2 $3 $1
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": working on it...\n"

	tstart=$(date +%s%6N)
	../build/simplefs < input/$fname.in > dummy_out
	tend=$(date +%s%6N)

	dt=$((tend - tstart))
	dt=$(bc <<< "scale=3; ${dt}/1000")

	goback 1
	printf "  [%2d/%2d] File \"%s\"" $2 $3 $1
	(($spacing > 0)) && printf ".%.0s" $(seq 1 $spacing)
	printf ": %6.3fms" $dt

	cmp --quiet dummy_out output/$fname.out
	res=$?

	rm dummy_out

	if [ $res -eq 0 ]; then
		printf " -> OK.\n"
	else
		printf " -> ERROR!\n\nTesting failed! :(\n"
		exit 1
	fi
}

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

printf "\nTesting succeded! :)\n"
