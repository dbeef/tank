## Deps:

* `protoc-gen-grpc-web` must be present in path;
visit [grpc-web](https://github.com/grpc/grpc-web/releases) to get the binary.
* gRPC artifact must be present in `CMAKE_PREFIX_PATH` - `server/deps/binaries/raspi` 
by default in `server/scripts/config-raspi.sh`
* `pigpio` - `sudo apt install libpgpio-dev` on the Raspberry

## Acknowledgments:

[Raspberry CMake toolchain file](https://github.com/Pro/raspi-toolchain)

