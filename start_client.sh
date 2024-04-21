#!/bin/bash

mkdir -p bin
rm -f bin/client
gcc -o bin/client client.c

bin/client 127.0.0.1
