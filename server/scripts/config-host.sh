#!/bin/sh -e

cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH=$PWD/deps/binaries/host \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_INSTALL_PREFIX=$PWD/install \
    --graphviz=depsgraph/depsgraph.dot
ln -f -s build/compile_commands.json .

