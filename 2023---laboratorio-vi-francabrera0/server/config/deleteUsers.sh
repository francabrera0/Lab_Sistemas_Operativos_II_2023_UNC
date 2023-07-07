#!/bin/bash

sudo sqlite3 ./database/users.db "SELECT USERNAME FROM USERS" | while read -r username; do 

    userdel -r "$username"
done