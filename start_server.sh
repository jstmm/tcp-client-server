#!/bin/bash

mkdir -p bin
rm -f bin/server
gcc -o bin/server server.c

bin/server
