#!/bin/bash

binary=./bin/program
file=./tests/log/computeResults

rm -f $file

$binary >> $file