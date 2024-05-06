.POSIX:
.SUFFIXES:

default:
	mkdir -p build
	gcc -g -o ./build/server ./src/server.c
	gcc -g -o ./build/client ./src/client.c
	gcc -g -o ./build/server_select ./src/server_select.c

.PHONY: clean
clean:
	rm -f ./build/*
