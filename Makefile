.POSIX:
.SUFFIXES:

default:
	mkdir -p bin
	gcc -g -o bin/server server.c
	gcc -g -o bin/client client.c
	gcc -g -o bin/server_select_multi server_select_multi.c

.PHONY: clean
clean:
	rm -f bin/*
