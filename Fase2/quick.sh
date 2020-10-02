#!/bin/bash

DIR=$(pwd)
ESC=1
read ESC
echo $ESC
for number in {1..30}
do
	CWD="./ep1 tests/trace1.txt $ESC output/fcfs/trace1_$number -d"
	echo $CWD
	$CWD
done
