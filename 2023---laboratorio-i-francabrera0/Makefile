CFLAGS = -Wall -pedantic -Werror -Wextra -Wconversion -std=gnu11
include = ./include
src = ./src
bin = ./bin
out = ./out
lib = ./lib
tmp = ./tmp

all: client1 client2 client3 server tmp

#________________________________Client1______________________________________________________________#
client1: $(bin)/client1
$(bin)/client1: $(out)/client1.o $(lib)/libclients.a
	mkdir -p $(bin)/
	gcc $(CFLAGS) -o $@ $(out)/client1.o -L$(lib)/ -lclients

#________________________________Client1.o____________________________________________________________#
$(out)/client1.o: $(src)/client/client1.c $(include)/client/client.h $(include)/utilities/msgStructs.h
	mkdir -p $(out)/
	gcc $(CFLAGS) -o $@ -c $(src)/client/client1.c

#________________________________Client2______________________________________________________________#
client2: $(bin)/client2
$(bin)/client2: $(out)/client2.o $(lib)/libclients.a
	gcc $(CFLAGS) -o $@ $(out)/client2.o -L$(lib)/ -lclients

#________________________________Client2.o____________________________________________________________#
$(out)/client2.o: $(src)/client/client2.c $(include)/client/client.h $(include)/utilities/msgStructs.h
	mkdir -p $(out)/
	gcc $(CFLAGS) -o $@ -c $(src)/client/client2.c

#________________________________Client3______________________________________________________________#
client3: $(bin)/client3
$(bin)/client3: $(out)/client3.o $(lib)/libclients.a
	mkdir -p $(bin)/
	gcc $(CFLAGS) -o $@ $(out)/client3.o -L$(lib)/ -lclients

#________________________________Client3.o____________________________________________________________#
$(out)/client3.o: $(src)/client/client3.c $(include)/client/client.h $(include)/utilities/msgStructs.h
	mkdir -p $(out)/
	gcc $(CFLAGS) -o $@ -c $(src)/client/client3.c

#________________________________Library for clients__________________________________________________#
$(lib)/libclients.a: $(out)/client.o $(out)/readConfig.o
	mkdir -p $(lib)/
	ar -rc $@ $^

$(out)/client.o: $(src)/client/client.c $(include)/client/client.h $(include)/utilities/msgStructs.h 
	gcc $(CFLAGS) -o $@ -c $(src)/client/client.c

$(out)/readConfig.o: $(src)/utilities/readConfig.c $(include)/utilities/readConfig.h 
	gcc $(CFLAGS) -o $@ -c $(src)/utilities/readConfig.c

#________________________________Server_______________________________________________________________#
server: $(bin)/server
$(bin)/server: $(src)/server/server.c $(src)/utilities/readConfig.c
	gcc $(CFLAGS) -o $@ $^

tmp:
	mkdir -p $(tmp)

#________________________________CLEAN________________________________________________________________#
clean:
	rm -rf $(bin)/ $(lib)/ $(out)/ $(tmp)/
