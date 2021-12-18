#!/bin/sh -e

grpcwebproxy --run_tls_server=false \
             --backend_addr=localhost:50051 \
             --backend_tls_noverify \
             --allow_all_origins 2>&1 > /tmp/grpc-web-proxy-logs.txt

