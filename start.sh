#!/bin/bash

if [[ $1 == "server" ]]; then

  bin/server

elif [[ $1 == "client" ]]; then

  bin/client 127.0.0.1

elif [[ $1 == "server_select_multi" ]]; then

  bin/server_select_multi 127.0.0.1

fi
