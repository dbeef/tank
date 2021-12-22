#!/bin/sh -e

cd $PWD/webapp/dist/
sudo python3 -m http.server 80 2>&1 > /tmp/webapp-logs.txt

