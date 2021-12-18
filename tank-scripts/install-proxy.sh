#!/bin/sh -e

sudo apt install git golang-go nodejs npm

GOPATH=~/go ; export GOPATH
git clone https://github.com/improbable-eng/grpc-web.git \
          $GOPATH/src/github.com/improbable-eng/grpc-web
cd $GOPATH/src/github.com/improbable-eng/grpc-web
go install ./go/grpcwebproxy
sudo ln -s $GOPATH/bin/grpcwebproxy /usr/local/bin/grpcwebproxy

