.POSIX:
.SUFFIXES:

default:
	mkdir -p bin
	gcc -g -o ./bin/server ./src/server.c
	gcc -g -o ./bin/client ./src/client.c
	gcc -g -o ./bin/server_select ./src/server_select.c
	gcc -g -o ./bin/server_poll ./src/server_poll.c

.PHONY: clean
clean:
	rm -f ./bin/*
