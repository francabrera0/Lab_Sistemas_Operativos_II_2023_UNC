#!/bin/bash

n=100

binary=./bin/program
file=./tests/log/timeResults

rm -f $file

for i in $(seq 1 $n)
do
    echo "Execution number $i" >> $file 
    sudo perf stat $binary 2>> $file
done
