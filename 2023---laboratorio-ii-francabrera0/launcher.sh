#!/bin/bash

gnome-terminal --title="Server" -- ./build/Server 1024 1025 sock

gnome-terminal --title="IPV4 Client" -- ./build/Client ipv4 localhost 1024

gnome-terminal --title="IPV6 Client" -- ./build/Client ipv6 ::1 1025

gnome-terminal --title="UNIX Client" -- ./build/Client unix sock 