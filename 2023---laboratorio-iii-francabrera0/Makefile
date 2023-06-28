CFLAGS = -g -Wall -Werror -pedantic -Wextra -Wconversion -std=gnu11 -O0
src = ./src
bin = ./bin
testlog = ./tests/log

$(bin)/program: $(src)/laboratorio3.c
	mkdir -p $(bin)/
	gcc $(CFLAGS) -o $@ $^

timeTest:$(bin)/program
	mkdir -p $(testlog)/
	sh ./tests/timeTest.sh
	py ./tests/timeTest.py

computeTest:$(bin)/program
	mkdir -p $(testlog)/
	sh ./tests/computeTest.sh
	py ./tests/computeTest.py

clean:
	rm -rf $(bin)/ $(testlog)/