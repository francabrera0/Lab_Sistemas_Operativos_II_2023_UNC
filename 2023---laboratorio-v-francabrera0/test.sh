#!/bin/bash

gcc -o ./src/test/test -fopenmp ./src/test/test.c -lcurl

gnome-terminal --title="Server" -- ./build/Server 8537
wait

./src/test/test 100

curl http://localhost:8537/print