printf "Running all tests:\n"

i=0
n=$(ls input -1 | wc -l)

for f in input/*.in; do
	((i++))
	fname=$(basename ${f%.in})

	printf "  [%d/%d] File \"%s\"... " $i $n $f
	out=$(../build/api_project < input/$fname.in | diff -u output/$fname.out -)

	if [ $? -eq 0 ]; then
		printf "OK.\n"
	else
		printf "ERROR!\n\nOutput from diff:\n%s\nTesting failed! :(\n"  $out
		exit 1
	fi
done

printf "\nTesting succeded! :)\n"
