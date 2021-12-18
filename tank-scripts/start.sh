#!/bin/sh -e
 
cd $1
./tank-scripts/run-server.sh &
./tank-scripts/run-proxy.sh &
./tank-scripts/run-webapp.sh &

