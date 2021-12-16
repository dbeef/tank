#!/bin/sh -e

export RASPI_TOOLCHAIN_FILE=$PWD/cmake/Toolchain-rpi.cmake
# Modify accordingly to your toolchain installation, more at:
# https://github.com/Pro/raspi-toolchain
export RASPBIAN_ROOTFS=/opt/cross-pi-gcc/rootfs-tank

cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=$RASPI_TOOLCHAIN_FILE \
    -DCMAKE_PREFIX_PATH=$PWD/deps/binaries/raspi \
    -DCMAKE_FIND_ROOT_PATH=$PWD/deps/binaries/raspi \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ln -f -s build/compile_commands.json .  
