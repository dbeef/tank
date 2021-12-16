#!/bin/sh -e

./tank-scripts/run-proxy.sh &
./tank-scripts/run-server.sh &
./tank-scripts/run-webapp.sh &

