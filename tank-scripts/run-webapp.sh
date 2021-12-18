#!/bin/sh -e

cd $PWD/webapp/dist/
python3 -m http.server 2>&1 > /tmp/webapp-logs.txt

