#!/bin/bash

n=16

file=./timeResults

rm -f $file

for i in $(seq 1 $n)
do
    echo "Execution with $i threads"
    ../build/Filter ../Sentinel2_20230208T140711_10m.tif $i >> $file
done

py test.py