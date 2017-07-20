#!/bin/bash

# File  : test_all.sh
# Author: Marco Bonelli
# Date  : 2017-07-20
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

printf "Running all tests:\n"

i=0
n=$(ls input -1 | wc -l)

for f in input/*.in; do
	((i++))
	fname=$(basename ${f%.in})

	printf "[%d/%d] File \"%s\"... " $i $n $f
	out=$(../build/simplefs < input/$fname.in | diff output/$fname.out -)

	if [ $? -eq 0 ]; then
		printf "OK.\n"
	else
		printf "ERROR!\n\n"
		printf "Output from diff:\n"
		printf "%s\n\n" "$out"
		printf "Testing failed! :(\n"
		exit 1
	fi
done

printf "\nTesting succeded! :)\n"
