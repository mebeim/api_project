#!/bin/bash

# File  : test_all.sh
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

function test_random {
	res=0
	printf "./random_fs.py -n %d | ../build/simplefs (x%d)... " $1 $2

	for i in $(seq 1 $2); do
		./random_fs.py -n $1 | ../build/simplefs > dummy_out
		./random_fs.py -n $1 -o | cmp --quiet dummy_out -

		if [ $? -ne 0 ]; then
			res=$?
			break
		fi
	done

	rm dummy_out

	if [ $res -eq 0 ]; then
		printf "OK.\n"
	else
		printf "ERROR!\n\nTesting failed! :(\n"
		exit 1
	fi
}

printf "Running all test files:\n"

i=0
n=$(ls input -1 | wc -l)

for f in input/*.in; do
	((i++))
	fname=$(basename ${f%.in})

	printf "[%2d/%2d] File \"%s\"... " $i $n $f
	../build/simplefs < input/$fname.in | cmp --quiet output/$fname.out -

	if [ $? -eq 0 ]; then
		printf "OK.\n"
	else
		printf "ERROR!\n\nTesting failed! :(\n"
		exit 1
	fi
done

printf "\nRunning random tests:\n"
test_random 10 100
test_random 100 10
test_random 1000 10
test_random 10000 1
test_random 100000 1

printf "\nTesting succeded! :)\n"
