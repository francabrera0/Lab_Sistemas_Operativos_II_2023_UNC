#!/bin/bash

gnome-terminal --title="Server" -- ./build/Server 8537

wait

gnome-terminal --title="Client1" -- ./build/Client client1 localhost 8537
gnome-terminal --title="Client2" -- ./build/Client client2 localhost 8537
