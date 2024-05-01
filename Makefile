.POSIX:
.SUFFIXES:

default:
	mkdir -p bin
	rm -f bin/*
	gcc -g -o bin/server server.c
	gcc -g -o bin/client client.c
