#!/bin/bash

for number in {1..30}
do
	CWD="./ep1 tests/trace1.txt 1 output/fcfs/trace1_$number -d"
	echo $CWD
	$CWD
done
