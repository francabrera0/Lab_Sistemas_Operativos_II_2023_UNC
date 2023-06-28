#!/bin/bash
gnome-terminal --title="Server" -- ./bin/server

sleep 1

gnome-terminal --title="Client1" -- ./bin/client1 proc1

gnome-terminal --title="Client2" -- ./bin/client2 proc2

gnome-terminal --title="Client3" -- ./bin/client3 proc3
