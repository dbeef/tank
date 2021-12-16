find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_library(Dependencies INTERFACE)
target_link_libraries(Dependencies INTERFACE
    gRPC::grpc++
    gRPC::grpc++_unsecure
    pigpio
)

