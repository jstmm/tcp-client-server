#!/bin/bash

if [[ $1 == "server" ]]; then

  bin/server

elif [[ $1 == "client" ]]; then

  bin/client 127.0.0.1

fi
